#ifndef SX126X_PORT_H
#define SX126X_PORT_H

#include <stdint.h>

#include "hal_gpio.h"

/// @brief The radio's control lines and BUSY timeout. The vendor driver treats
///        this as an opaque `const void*` context, so a board with different
///        wiring only changes this struct. The SPI peripheral and chip-select
///        pin live in the SPI HAL, so this port drives one radio.
typedef struct
{
    hal_gpio_id_t reset;      //!< Radio NRESET line (active low).
    hal_gpio_id_t busy;       //!< Radio BUSY line (input; high while busy).
    uint32_t busy_timeout_ms; //!< Max time to wait for BUSY to fall.
} sx126x_port_t;

/// @brief Fills `port` with this board's SX126x wiring and default timeout,
///        initializes the reset/BUSY/DIO1 lines and the SPI peripheral, and
///        leaves the radio deselected. Call once before any driver call.
/// @param port Port to initialize.
void sx126x_port_init(sx126x_port_t *port);

#endif // SX126X_PORT_H