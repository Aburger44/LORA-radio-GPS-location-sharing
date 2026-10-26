#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>
/// @brief provides current time
/// @param  
/// @return uint64 of time in clock cycles
uint64_t hal_timer_get_time_us(void);

#endif // HAL_TIMER_H