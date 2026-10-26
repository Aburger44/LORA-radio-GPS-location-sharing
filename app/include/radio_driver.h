#ifndef RADIO_DRIVER_H
#define RADIO_DRIVER_H

/// @brief Initializes the SX126x port and brings the radio up through the
///        vendor driver: reset, standby, and the LoRa packet type.
void radio_driver_init(void);

/// @brief FreeRTOS task entry point for the radio: calls radio_driver_init()
///        once, then prints the chip status and device errors once per second.
/// @param params Unused; the task takes no parameters.
void radio_driver_task(void *params);

#endif // RADIO_DRIVER_H