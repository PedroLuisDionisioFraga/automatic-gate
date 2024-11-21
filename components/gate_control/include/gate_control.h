#ifndef GATE_CONTROL_H
#define GATE_CONTROL_H

#include "gpio_control.h"

typedef enum {
    GATE_STATE_STOPPED,
    GATE_STATE_OPENING,
    GATE_STATE_CLOSING
} gate_state_t;

typedef struct {
    gpio_cfg_t button;
    gpio_cfg_t sensor_open;
    gpio_cfg_t sensor_closed;
    gpio_cfg_t led_open;
    gpio_cfg_t led_closed;
    gpio_cfg_t led_moving;
    gpio_cfg_t motor;
} gate_config_t;

void configure_gate(gate_config_t gate_cfg);
void handle_button_press(gate_config_t gate_cfg);
void check_end_stops(gate_config_t gate_cfg);
void update_leds(gate_config_t gate_cfg);

#endif // GATE_CONTROL_H