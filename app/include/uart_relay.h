#ifndef UART_RELAY_H
#define UART_RELAY_H

/// @brief Stores the current task's handle for the RX interrupt and enables the
///        UART RX interrupt. Called by uart_relay_task(), from the task that
///        will drain the received lines.
void uart_relay_init(void);

/// @brief FreeRTOS task entry point for the GPS link: enables the UART RX
///        interrupt, then blocks on a task notification until the ISR hands
///        over a complete NMEA sentence, prints it, and prints the decoded
///        position. Never returns.
/// @param params Unused; the task takes no parameters.
void uart_relay_task(void *params);

#endif // UART_RELAY_H