#include "hal_uart.h"

#include <stdbool.h>

#include "hardware/gpio.h"
#include "hardware/uart.h"

#define GPS_RX_PIN 17
#define GPS_BAUDRATE 38400

void hal_uart_init(hal_uart_rx_callback_t cb)
{
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);
    uart_init(uart0, GPS_BAUDRATE);

    // Bind the vector table entry to our handler and let the UART raise an IRQ
    // whenever a received byte is waiting.
    if (cb != NULL)
    {
        irq_set_exclusive_handler(UART0_IRQ, cb);
        irq_set_enabled(UART0_IRQ, true);
        uart_set_irqs_enabled(uart0, true, false);
    }
}

bool hal_uart_is_readable(void)
{
    return uart_is_readable(uart0);
}

char hal_uart_getc(void)
{
    return uart_getc(uart0);
    // char c = *((volatile uint8_t *)0x40034000);
    // return c;
}