// This is rp2040/hal_gpio.c, the implementation file for the GPIO HAL
#include "hal_gpio.h"

#include <stdbool.h>
#include <stdint.h>

#include "hardware/gpio.h"

#define WHITE_LED_PIN 24
#define RED_LED_PIN 23
#define SX126X_RESET_PIN 8
#define SX126X_BUSY_PIN 15
#define SX126X_DIO1_PIN 9

static const uint32_t pin[HAL_GPIO_NUM] = {
    [HAL_GPIO_WHITE_LED] = WHITE_LED_PIN,
    [HAL_GPIO_RED_LED]   = RED_LED_PIN,
    [HAL_GPIO_SX126X_RESET] = SX126X_RESET_PIN,
    [HAL_GPIO_SX126X_BUSY]  = SX126X_BUSY_PIN,
    [HAL_GPIO_SX126X_DIO1]  = SX126X_DIO1_PIN,
};

void hal_gpio_init(hal_gpio_id_t id)
{
    if (id >= HAL_GPIO_NUM)
    {
        return;
    }

    gpio_init(pin[id]);
    gpio_set_dir(pin[id], GPIO_OUT);
    gpio_put(pin[id], 0); // default state: low
}

void hal_gpio_init_input(hal_gpio_id_t id)
{
    if (id >= HAL_GPIO_NUM)
    {
        return;
    }

    gpio_init(pin[id]);
    gpio_set_dir(pin[id], GPIO_IN);
}

void hal_gpio_set(hal_gpio_id_t id, bool value)
{
    gpio_put(pin[id], value);
}

bool hal_gpio_get(hal_gpio_id_t id)
{
    return gpio_get(pin[id]);
}
