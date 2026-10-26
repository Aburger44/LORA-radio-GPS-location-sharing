#include "hal_spi.h"

#include <stddef.h>
#include <stdint.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"

#define SPI_PORT spi1
#define SPI_SCK_PIN 14
#define SPI_SDO_PIN 11
#define SPI_SDI_PIN 12
#define SPI_CS_PIN 10
#define SPI_BAUDRATE_HZ 1000000

void hal_spi_init(void)
{
    spi_init(SPI_PORT, SPI_BAUDRATE_HZ);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SDO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SDI_PIN, GPIO_FUNC_SPI);

    // The SPI peripheral does not drive chip-select in controller mode, so we own
    // it as a plain output and hold it high (deselected) between transactions.
    gpio_init(SPI_CS_PIN);
    gpio_put(SPI_CS_PIN, true); // preload the latch so driving the pin never glitches it low
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
}

void hal_spi_select(void)
{
    gpio_put(SPI_CS_PIN, false);
}

void hal_spi_deselect(void)
{
    gpio_put(SPI_CS_PIN, true);
}

void hal_spi_transfer(const uint8_t *tx, uint8_t *rx, size_t len)
{
    if ((tx == NULL) || (len == 0u))
    {
        return;
    }

    hal_spi_select();
    if (rx != NULL)
    {
        spi_write_read_blocking(SPI_PORT, tx, rx, len);
    }
    else
    {
        spi_write_blocking(SPI_PORT, tx, len);
    }
    hal_spi_deselect();
}

void hal_spi_read(const uint8_t *cmd, size_t cmd_len, uint8_t *rx, size_t rx_len)
{
    if ((cmd == NULL) || (cmd_len == 0u) || (rx == NULL) || (rx_len == 0u))
    {
        return;
    }

    hal_spi_select();
    spi_write_blocking(SPI_PORT, cmd, cmd_len);
    spi_read_blocking(SPI_PORT, 0x00, rx, rx_len);
    hal_spi_deselect();
}

void hal_spi_write(const uint8_t *cmd, size_t cmd_len, const uint8_t *data, size_t data_len)
{
    if ((cmd == NULL) || (cmd_len == 0u))
    {
        return;
    }

    hal_spi_select();
    spi_write_blocking(SPI_PORT, cmd, cmd_len);
    if ((data != NULL) && (data_len != 0u))
    {
        spi_write_blocking(SPI_PORT, data, data_len);
    }
    hal_spi_deselect();
}