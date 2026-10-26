#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// FreeRTOSConfig.h for the M10 FreeRTOS task demo.
//
// Written against FreeRTOS-Kernel V11.3.1 (RP2040 port,
// portable/ThirdParty/GCC/RP2040) with Pico SDK 2.2.0 on board "pico".
//
// This build uses static allocation only. The FreeRTOS-Kernel-Static CMake
// target already passes -DconfigSUPPORT_STATIC_ALLOCATION=1 and
// -DconfigKERNEL_PROVIDED_STATIC_MEMORY=1, so this file does NOT redefine
// them; it only opts out of dynamic allocation.

/* ---- Scheduler ---------------------------------------------------------- */
#define configUSE_PREEMPTION                    1
#define configUSE_TICKLESS_IDLE                 0
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_TIME_SLICING                  1
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                ( ( configSTACK_DEPTH_TYPE ) 256 )
#define configSTACK_DEPTH_TYPE                  uint32_t
#define configUSE_16_BIT_TICKS                  0   /* select a 32-bit TickType_t */
#define configIDLE_SHOULD_YIELD                 1

/* ---- Both RP2040 cores -------------------------------------------------- */
#define configNUMBER_OF_CORES                   2
/* configUSE_PASSIVE_IDLE_HOOK is mandatory once configNUMBER_OF_CORES > 1. */
#define configUSE_PASSIVE_IDLE_HOOK             0
/* configTICK_CORE is left undefined: rp2040_config.h defaults it to 0 when
   there is more than one core. */
/* configUSE_PORT_OPTIMISED_TASK_SELECTION must stay 0/undefined; it is a
   compile error when configNUMBER_OF_CORES > 1. */
/* Core affinity must be on for the SDK's flash driver: pico_flash/flash.c
   raises #error when configUSE_CORE_AFFINITY is 0 while the SDK's
   PICO_FLASH_SAFE_EXECUTE_SUPPORT_FREERTOS_SMP is set, and the SDK sets that
   whenever LIB_FREERTOS_KERNEL && FREE_RTOS_KERNEL_SMP (both defined by the
   RP2040 port). */
#define configUSE_CORE_AFFINITY                 1

/* ---- Memory: static only ------------------------------------------------ */
#define configSUPPORT_DYNAMIC_ALLOCATION        0

/* ---- Queues: static-allocation only -------------------------------------- *
 * Queues need no enabling here: queue.c is part of the kernel core and is
 * already linked, which is why queue.c.obj shows up in the build log.
 *
 * With no heap, a queue is created with xQueueCreateStatic() plus a
 * caller-owned StaticQueue_t and storage area, exactly as main.c creates its
 * tasks: the same static-allocation rule this project already follows.
 *
 * The queue registry is on so a debugger can show queue names. Queue sets and
 * counting semaphores stay off because nothing here uses them.              */
#define configUSE_QUEUE_SETS                    0
#define configUSE_COUNTING_SEMAPHORES           0
#define configQUEUE_REGISTRY_SIZE               8

/* ---- Software timers: required on RP2040 while sync interop is on -------- *
 * pico_stdlib pulls in pico_time/pico_sync, which makes the port auto-enable
 * configSUPPORT_PICO_SYNC_INTEROP. That path calls xEventGroupSetBitsFromISR(),
 * which only exists when configUSE_TIMERS == 1 and
 * INCLUDE_xTimerPendFunctionCall == 1. Hence software timers must be enabled
 * here even though the application itself creates none.                       */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            256   /* words, not bytes */
#define configUSE_EVENT_GROUPS                  1

/* ---- Hooks (kept off so no hook symbol is required) --------------------- */
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_MALLOC_FAILED_HOOK            0

/* ---- Co-routines / stats (off) ------------------------------------------ */
#define configUSE_CO_ROUTINES                   0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* ---- API inclusion ------------------------------------------------------ */
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskDelayUntil                 1
#define INCLUDE_xTimerPendFunctionCall          1   /* pulls in xEventGroupSetBitsFromISR */
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_xTaskGetSchedulerState          1

#include <assert.h>
#define configASSERT( x )                       assert( x )

#endif /* FREERTOS_CONFIG_H */