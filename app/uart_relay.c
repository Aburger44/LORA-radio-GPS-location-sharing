#include "uart_relay.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "minmea.h"
#include "hal_uart.h"

// Ping-pong (double) buffers: the ISR fills one while the task drains the
// other. Two buffers only hold one line ahead, and the task can lose a line in
// two different ways once it falls a whole line behind:
//
//   1. Coalescing. A second line completing before the task consumes the first
//      leaves only one pending notification, so the newest line is processed
//      and the older complete line is dropped, not overwritten.
//   2. Wrap-around. If the task is still printing a line when two more
//      completions arrive, the ISR ping-pongs back onto the slot being printed
//      and overwrites it in place.
//
// Falling a line behind is realistic: a GPS epoch is 5-10 sentences arriving
// back-to-back at 38400 baud, so two completions can land inside one processing
// window while a USB printf is in flight. A ring buffer is the general fix.
#define LINE_BUFFER_COUNT 2

static char line_buffers[LINE_BUFFER_COUNT][MINMEA_MAX_SENTENCE_LENGTH];

// Index of the buffer the ISR is currently filling. Only the ISR touches it.
static int isr_buffer;

// Number of bytes stored in the buffer the ISR is filling. Only the ISR uses it.
static size_t line_len;

// Task the RX interrupt notifies when a line is complete. Set once by the task
// before the interrupt is enabled, then only read by the ISR.
static TaskHandle_t uart_rx_task_handle;

/// @brief Parses one complete NMEA sentence and prints the fields we care about.
static void nmea_report(const char *sentence)
{
    switch (minmea_sentence_id(sentence, false))
    {
    case MINMEA_SENTENCE_GGA:
    {
        struct minmea_sentence_gga frame;
        if ((!minmea_parse_gga(&frame, sentence)) || (frame.fix_quality == 0))
        {
            return;
        }

        float lat = minmea_tocoord(&frame.latitude);
        float lon = minmea_tocoord(&frame.longitude);
        if ((!isfinite(lat)) || (!isfinite(lon)))
        {
            return; // no fix yet: tocoord() signals "missing" with NaN
        }

        printf("[GPS] time=%02d:%02d:%02d lat=%.6f lon=%.6f\n",
               frame.time.hours,
               frame.time.minutes,
               frame.time.seconds,
               lat,
               lon);
        break;
    }
    default:
        break;
    }
}

/// @brief UART RX interrupt handler: buffers received bytes into the current
///        ping-pong buffer until it sees the end of a line, then notifies the
///        GPS task, passing it the slot that was completed.
static void uart_rx_isr(void)
{
    // The handle is written once, before the RX interrupt is enabled, and never
    // again; the snapshot just keeps this invocation's view of it stable. Only
    // the ...FromISR() APIs are legal in an interrupt handler.
    TaskHandle_t handle = uart_rx_task_handle;

    while (hal_uart_is_readable())
    {
        char c = hal_uart_getc();

        if ((c == '\n') || (c == '\0'))
        {
            uint32_t slot = (uint32_t)isr_buffer; // the slot this line completes

            line_buffers[slot][line_len] = '\0';
            isr_buffer ^= 1; // ping-pong to the other buffer
            line_len = 0;

            if (handle != NULL)
            {
                // The slot index travels in the notification's value, so the
                // task never has to read shared state to find the line.
                BaseType_t higher_priority_task_woken = pdFALSE;
                xTaskNotifyFromISR(handle, slot, eSetValueWithOverwrite,
                                   &higher_priority_task_woken);
                portYIELD_FROM_ISR(higher_priority_task_woken);
            }
        }
        else if ((c != '\r') && (line_len < (sizeof(line_buffers[0]) - 1u)))
        {
            line_buffers[isr_buffer][line_len] = c;
            line_len++;
        }
        else
        {
            // dropped: '\r', or the line is full
        }
    }
}

void uart_relay_init(void)
{
    // Remember which task the RX interrupt should notify. Stored before the
    // interrupt is enabled, so the ISR never sees an unset handle.
    uart_rx_task_handle = xTaskGetCurrentTaskHandle();

    hal_uart_init(&uart_rx_isr); // enables the RX interrupt; may fire as soon as it returns
}

void uart_relay_task(void *params)
{
    (void)params;

    uart_relay_init();

    for (;;)
    {
        uint32_t slot;

        // Block until the ISR completes a line; the notification carries the
        // slot index. With eSetValueWithOverwrite, if two lines complete before
        // this task wakes, only the newest slot survives and the older
        // completed line is dropped.
        (void)xTaskNotifyWait(0, 0, &slot, portMAX_DELAY);

        printf("%s\n", line_buffers[slot]);
        nmea_report(line_buffers[slot]);
    }
}