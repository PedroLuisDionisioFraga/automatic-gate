#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include "driver/gpio.h"
#include "esp_err.h"

typedef struct {
	gpio_num_t pin;           	// Número do pino GPIO
	gpio_mode_t mode;         	// Modo do GPIO (INPUT, OUTPUT, etc.)
	gpio_pullup_t pullup;    	// Resistores de pull-up
	gpio_pulldown_t pulldown;  	// Resistores de pull-down
} gpio_cfg_t;

// Funções públicas
esp_err_t gpio_set_cfg(gpio_cfg_t cfg);
gpio_cfg_t *gpio_get_cfg(gpio_num_t pin);
esp_err_t gpio_write(gpio_num_t pin, uint32_t state);
esp_err_t gpio_toggle(gpio_num_t pin);
uint32_t gpio_read(gpio_num_t pin);

#endif // GPIO_CONTROL_H