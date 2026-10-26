#include "sx126x_hal.h"

#include <stdbool.h>
#include <stdint.h>

#include "hal_gpio.h"
#include "hal_spi.h"
#include "hal_timer.h"
#include "sx126x_port.h"

// Reset pulse and settle times. The datasheet only requires NRESET low for
// ~100 us, but reference ports use a longer, safer pulse.
#define SX126X_RESET_LOW_MS 10
#define SX126X_RESET_HIGH_MS 10

// A falling edge on NSS wakes a sleeping radio; hold it low for a moment.
#define SX126X_WAKEUP_LOW_MS 1

// Worst-case BUSY time for a real transition is a few milliseconds, so this is
// generous. It exists so a missing or stuck BUSY line cannot hang the CPU.
#define SX126X_BUSY_TIMEOUT_MS 100

/// @brief Busy-waits for the given number of milliseconds.
static void delay_ms(uint32_t ms)
{
    const uint64_t deadline = hal_timer_get_time_us() + (uint64_t)ms * 1000U;
    while (hal_timer_get_time_us() < deadline)
    {
    }
}

/// @brief Waits until the radio's BUSY line is low.
/// @param port The radio port.
/// @return true if the radio became ready, false if the timeout expired.
static bool wait_busy(const sx126x_port_t *port)
{
    const uint64_t deadline = hal_timer_get_time_us() + (uint64_t)port->busy_timeout_ms * 1000U;
    while (hal_gpio_get(port->busy))
    {
        if (hal_timer_get_time_us() >= deadline)
        {
            return false;
        }
    }
    return true;
}

void sx126x_port_init(sx126x_port_t *port)
{
    port->reset = HAL_GPIO_SX126X_RESET;
    port->busy = HAL_GPIO_SX126X_BUSY;
    port->busy_timeout_ms = SX126X_BUSY_TIMEOUT_MS;

    hal_gpio_init(port->reset);
    hal_gpio_init_input(port->busy);
    hal_gpio_init_input(HAL_GPIO_SX126X_DIO1); // reserved for the next module
    hal_spi_init();

    // hal_gpio_init() leaves an output low, which would hold the active-low
    // NRESET asserted. Release it so the radio runs until the driver resets it.
    hal_gpio_set(port->reset, true);
}

sx126x_hal_status_t sx126x_hal_write(const void *context, const uint8_t *command, const uint16_t command_length,
                                     const uint8_t *data, const uint16_t data_length)
{
    const sx126x_port_t *port = (const sx126x_port_t *)context;

    if (!wait_busy(port))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    hal_spi_write(command, (size_t)command_length, data, (size_t)data_length);
    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read(const void *context, const uint8_t *command, const uint16_t command_length,
                                    uint8_t *data, const uint16_t data_length)
{
    const sx126x_port_t *port = (const sx126x_port_t *)context;

    if (!wait_busy(port))
    {
        return SX126X_HAL_STATUS_ERROR;
    }

    hal_spi_read(command, (size_t)command_length, data, (size_t)data_length);
    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset(const void *context)
{
    const sx126x_port_t *port = (const sx126x_port_t *)context;

    hal_gpio_set(port->reset, false);
    delay_ms(SX126X_RESET_LOW_MS);
    hal_gpio_set(port->reset, true);
    delay_ms(SX126X_RESET_HIGH_MS);

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup(const void *context)
{
    const sx126x_port_t *port = (const sx126x_port_t *)context;

    // Wake from sleep is triggered by a falling edge on NSS. Pull it low long
    // enough for the radio to see it, then release it without clocking a byte.
    hal_spi_select();
    delay_ms(SX126X_WAKEUP_LOW_MS);
    hal_spi_deselect();

    // Wake is not the same as ready: wait for BUSY to fall before returning.
    return wait_busy(port) ? SX126X_HAL_STATUS_OK : SX126X_HAL_STATUS_ERROR;
}