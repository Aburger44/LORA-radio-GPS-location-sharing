// This is include/hal_gpio.h, the header file for the GPIO HAL
#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

typedef enum
{
    HAL_GPIO_WHITE_LED = 0,
    HAL_GPIO_RED_LED,
    HAL_GPIO_SX126X_RESET,
    HAL_GPIO_SX126X_BUSY,
    HAL_GPIO_SX126X_DIO1,
    HAL_GPIO_NUM // automatically equal to the number of instances!
} hal_gpio_id_t;

/// @brief Initializes the GPIO instance to its default state (low).
/// @param id Which GPIO instance to initialize.
void hal_gpio_init(hal_gpio_id_t id);

/// @brief Initializes the GPIO instance as an input.
/// @param id Which GPIO instance to initialize.
void hal_gpio_init_input(hal_gpio_id_t id);

/// @brief Sets the value of the specified GPIO instance.
/// @param id Which GPIO instance to set.
/// @param value The value to set the GPIO to (true for high, false for low).
void hal_gpio_set(hal_gpio_id_t id, bool value);

/// @brief Gets the value of the specified GPIO instance.
/// @param id Which GPIO instance to get.
/// @return The value of the GPIO instance (true for high, false for low).
bool hal_gpio_get(hal_gpio_id_t id);

#endif // HAL_GPIO_H