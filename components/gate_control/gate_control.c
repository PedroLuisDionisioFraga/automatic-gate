#include "gate_control.h"
#include <esp_log.h>

static gate_state_t current_state = GATE_STATE_STOPPED;

void configure_gate(gate_config_t gate_cfg) {
    if(gpio_set_cfg(gate_cfg.button) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure button");
    if(gpio_set_cfg(gate_cfg.sensor_open) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure sensor_open");
    if(gpio_set_cfg(gate_cfg.sensor_closed) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure sensor_closed");
    if(gpio_set_cfg(gate_cfg.led_open) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure led_open");
    if(gpio_set_cfg(gate_cfg.led_closed) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure led_closed");
    if(gpio_set_cfg(gate_cfg.led_moving) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure led_moving");
    if(gpio_set_cfg(gate_cfg.motor) != ESP_OK)
        ESP_LOGE("GATE_CONTROL", "Failed to configure motor");
}

void handle_button_press(gate_config_t gate_cfg) {
    switch (current_state) {
        case GATE_STATE_STOPPED:
            if (gpio_read(gate_cfg.sensor_open.pin) == 0) {
                current_state = GATE_STATE_CLOSING;
                gpio_write(gate_cfg.motor.pin, 1);
            } else if (gpio_read(gate_cfg.sensor_closed.pin) == 0) {
                current_state = GATE_STATE_OPENING;
                gpio_write(gate_cfg.motor.pin, 1);
            }
            break;

        case GATE_STATE_OPENING:
            current_state = GATE_STATE_STOPPED;
            gpio_write(gate_cfg.motor.pin, 0);
            break;

        case GATE_STATE_CLOSING:
            current_state = GATE_STATE_STOPPED;
            gpio_write(gate_cfg.motor.pin, 0);
            break;

        default:
            break;
    }

    update_leds(gate_cfg);
}

void check_end_stops(gate_config_t gate_cfg) {
    if (gpio_read(gate_cfg.sensor_open.pin) == 0 && current_state == GATE_STATE_OPENING) {
        current_state = GATE_STATE_STOPPED;
        gpio_write(gate_cfg.motor.pin, 0);
    }
    if (gpio_read(gate_cfg.sensor_closed.pin) == 0 && current_state == GATE_STATE_CLOSING) {
        current_state = GATE_STATE_STOPPED;
        gpio_write(gate_cfg.motor.pin, 0);
    }
    update_leds(gate_cfg);
}

void update_leds(gate_config_t gate_cfg) {
    switch (current_state) {
        case GATE_STATE_STOPPED:
            gpio_write(gate_cfg.led_moving.pin, 0);
            gpio_write(gate_cfg.led_open.pin, (gpio_read(gate_cfg.sensor_open.pin) == 0));
            gpio_write(gate_cfg.led_closed.pin, (gpio_read(gate_cfg.sensor_closed.pin) == 0));
            break;

        case GATE_STATE_OPENING:
        case GATE_STATE_CLOSING:
            gpio_write(gate_cfg.led_open.pin, 0);
            gpio_write(gate_cfg.led_closed.pin, 0);
            gpio_toggle(gate_cfg.led_moving.pin);
            break;

        default:
            break;
    }
}