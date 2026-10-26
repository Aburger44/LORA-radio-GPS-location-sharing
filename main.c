#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "radio_driver.h"
#include "uart_relay.h"

// Task stack sizes, in WORDS, not bytes: StackType_t is uint32_t on the RP2040,
// so 1024 words is 4096 bytes per task.
#define RADIO_TASK_STACK_WORDS 1024U
#define GPS_TASK_STACK_WORDS   1024U

// Static storage for the two tasks. There is no heap, so the stack array and
// the TCB are declared here and handed to xTaskCreateStatic().
static StackType_t radio_task_stack[RADIO_TASK_STACK_WORDS];
static StaticTask_t radio_task_tcb;

static StackType_t gps_task_stack[GPS_TASK_STACK_WORDS];
static StaticTask_t gps_task_tcb;

/// @brief Creates the radio and GPS tasks with static storage and starts the
///        FreeRTOS scheduler.
/// @return Never returns: the RP2040 port does not come back from
///         vTaskStartScheduler().
int main(void)
{
    stdio_init_all();

    // xTaskCreateStatic() returns NULL when it cannot create the task, so keep
    // the handle and check it rather than discarding the result.
    //
    // The radio driver has the slower 1 Hz status loop, so it runs below the
    // GPS task, which has to keep up with the NMEA byte stream.
    TaskHandle_t radio_task = xTaskCreateStatic(radio_driver_task, "lora",
                                                RADIO_TASK_STACK_WORDS, NULL,
                                                tskIDLE_PRIORITY + 1,
                                                radio_task_stack,
                                                &radio_task_tcb);
    configASSERT(radio_task != NULL);
    (void)radio_task; // configASSERT() is compiled out when NDEBUG is set

    TaskHandle_t gps_task = xTaskCreateStatic(uart_relay_task, "gps",
                                              GPS_TASK_STACK_WORDS, NULL,
                                              tskIDLE_PRIORITY + 2,
                                              gps_task_stack, &gps_task_tcb);
    configASSERT(gps_task != NULL);
    (void)gps_task; // configASSERT() is compiled out when NDEBUG is set

    vTaskStartScheduler();

    // The scheduler never returns on this port; spin instead of falling off
    // the end of main().
    for (;;)
    {
    }
}