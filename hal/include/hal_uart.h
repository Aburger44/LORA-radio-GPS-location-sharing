#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdbool.h>
#include <stdint.h>

void hal_uart_init();
bool hal_uart_is_readable();
char hal_uart_getc();

typedef void (*hal_uart_rx_callback_t)(void);

void hal_uart_init(hal_uart_rx_callback_t cb);

bool hal_uart_is_readable(void);
char hal_uart_getc(void);

#endif // HAL_UART_H