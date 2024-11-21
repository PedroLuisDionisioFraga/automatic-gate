#include "gate_pinout.h"

void set_gate_pinout(gate_config_t *gate_cfg) {
    
    gate_cfg->button = (gpio_cfg_t){
        .pin = GPIO_NUM_18,
        .mode = GPIO_MODE_INPUT,
        .pullup = GPIO_PULLUP_ENABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->sensor_open = (gpio_cfg_t){
        .pin = GPIO_NUM_19,
        .mode = GPIO_MODE_INPUT,
        .pullup = GPIO_PULLUP_ENABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->sensor_closed = (gpio_cfg_t){
        .pin = GPIO_NUM_21,
        .mode = GPIO_MODE_INPUT,
        .pullup = GPIO_PULLUP_ENABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->led_open = (gpio_cfg_t){
        .pin = GPIO_NUM_25,
        .mode = GPIO_MODE_OUTPUT,
        .pullup = GPIO_PULLUP_DISABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->led_closed = (gpio_cfg_t){
        .pin = GPIO_NUM_26,
        .mode = GPIO_MODE_OUTPUT,
        .pullup = GPIO_PULLUP_DISABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->led_moving = (gpio_cfg_t){
        .pin = GPIO_NUM_27,
        .mode = GPIO_MODE_OUTPUT,
        .pullup = GPIO_PULLUP_DISABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

    gate_cfg->motor = (gpio_cfg_t){
        .pin = GPIO_NUM_33,
        .mode = GPIO_MODE_OUTPUT,
        .pullup = GPIO_PULLUP_DISABLE,
        .pulldown = GPIO_PULLDOWN_DISABLE,
    };

}