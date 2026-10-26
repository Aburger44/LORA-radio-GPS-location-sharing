#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stddef.h>
#include <stdint.h>

/// @brief Initializes the SPI peripheral (mode 0, MSB-first, 8-bit) and its
///        SCK, SDO, and SDI pins. The chip-select line is configured as a
///        plain GPIO output held high, so the device starts deselected.
void hal_spi_init(void);

/// @brief Asserts chip-select (drives it low) to start a transaction frame.
///        Used by drivers that must control the frame boundary themselves, for
///        example to hold the line low while a slave wakes up.
void hal_spi_select(void);

/// @brief Releases chip-select (drives it high) to end a transaction frame.
void hal_spi_deselect(void);

/// @brief Runs one SPI transaction framed by chip-select: pulls chip-select
///        low, clocks `len` bytes from `tx` while capturing `len` bytes into
///        `rx`, then releases chip-select.
/// @param tx Bytes to transmit. Must not be NULL.
/// @param rx Buffer for the received bytes, or NULL to ignore them.
/// @param len Number of bytes to transfer.
void hal_spi_transfer(const uint8_t *tx, uint8_t *rx, size_t len);

/// @brief Runs one SPI transaction framed by chip-select that sends a command
///        and then reads its reply: pulls chip-select low, clocks `cmd_len`
///        bytes from `cmd`, then clocks `rx_len` bytes of 0x00 (NOP) while
///        capturing the reply into `rx`, then releases chip-select.
/// @param cmd Command bytes to transmit. Must not be NULL.
/// @param cmd_len Number of command bytes.
/// @param rx Buffer for the received bytes. Must not be NULL.
/// @param rx_len Number of reply bytes to clock.
void hal_spi_read(const uint8_t *cmd, size_t cmd_len, uint8_t *rx, size_t rx_len);

/// @brief Runs one SPI transaction framed by chip-select that sends a command
///        followed by a data block in the same frame: pulls chip-select low,
///        clocks `cmd_len` bytes from `cmd`, then `data_len` bytes from `data`,
///        then releases chip-select. No reply is captured.
/// @param cmd Command bytes to transmit. Must not be NULL.
/// @param cmd_len Number of command bytes.
/// @param data Data bytes to transmit, or NULL when `data_len` is zero.
/// @param data_len Number of data bytes.
void hal_spi_write(const uint8_t *cmd, size_t cmd_len, const uint8_t *data, size_t data_len);

#endif // HAL_SPI_H