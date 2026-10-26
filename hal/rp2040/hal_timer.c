#include "hal_timer.h"
#include <stdint.h>
#include "pico/time.h"

uint64_t hal_timer_get_time_us(void) {
    uint64_t time = time_us_64();
    return time;
}