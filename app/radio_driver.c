#include "radio_driver.h"

#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "sx126x.h"
#include "sx126x_driver_version.h"
#include "sx126x_port.h"

// How often to print the radio status.
#define STATUS_PERIOD_MS 1000

// The radio's wiring and BUSY timeout. Every vendor driver call takes a pointer
// to this as its context, which the vendor driver hands back to sx126x_hal.c.
// Static so it outlives radio_driver_init() for the status loop.
static sx126x_port_t radio_port;

// The driver decodes the SX126x Stat1 byte into two 3-bit fields (see
// sx126x.h). The loop prints them as plain numbers; read the names off this
// table by hand.
//
//   chip_mode     0 = unused, 1 = RFU, 2 = STDBY_RC, 3 = STDBY_XOSC,
//                 4 = FS, 5 = RX, 6 = TX, 7 = RFU
//   cmd_status    0 = reserved, 1 = RFU, 2 = data available,
//                 3 = command timeout, 4 = processing error, 5 = failure,
//                 6 = TX done, 7 = RFU

/// @brief Prints the chip mode and command status from one status read.
static void print_status(void)
{
    sx126x_chip_status_t status;

    if (sx126x_get_status(&radio_port, &status) != SX126X_STATUS_OK)
    {
        printf("[RADIO] get_status failed\n");
        return;
    }

    printf("[RADIO] chip_mode=%d cmd_status=%d\n", status.chip_mode,
                   status.cmd_status);
}

/// @brief Prints the radio's latched device-error flags.
static void print_errors(void)
{
    sx126x_errors_mask_t errors;

    if (sx126x_get_device_errors(&radio_port, &errors) != SX126X_STATUS_OK)
    {
        printf("[RADIO] get_device_errors failed\n");
        return;
    }

    printf("[RADIO] device errors=0x%04X\n", (unsigned)errors);
}

void radio_driver_init(void)
{
    sx126x_port_init(&radio_port);

    printf("[RADIO] Semtech SX126x driver %s\n", sx126x_driver_version_get_version_string());

    if (sx126x_reset(&radio_port) != SX126X_STATUS_OK)
    {
        printf("[RADIO] reset failed\n");
    }

    if (sx126x_set_standby(&radio_port, SX126X_STANDBY_CFG_RC) != SX126X_STATUS_OK)
    {
        printf("[RADIO] set_standby failed\n");
    }

    // Exercise a write command followed by a read-back: set the LoRa packet
    // type and confirm the driver reads the same value back.
    sx126x_pkt_type_t pkt_type = SX126X_PKT_TYPE_GFSK;
    if (sx126x_set_pkt_type(&radio_port, SX126X_PKT_TYPE_LORA) != SX126X_STATUS_OK)
    {
        printf("[RADIO] set_pkt_type failed\n");
    }
    if (sx126x_get_pkt_type(&radio_port, &pkt_type) == SX126X_STATUS_OK)
    {
        printf("[RADIO] packet type=%u (1=LoRa)\n", (unsigned)pkt_type);
    }
}

void radio_driver_task(void *params)
{
    (void)params;

    radio_driver_init();

    // xTaskDelayUntil() waits from the last wake time, so the 1 s period does
    // not drift by however long printing the status took.
    TickType_t last_wake = xTaskGetTickCount();

    for (;;)
    {
        print_status();
        print_errors();

        (void)xTaskDelayUntil(&last_wake, pdMS_TO_TICKS(STATUS_PERIOD_MS));
    }
}