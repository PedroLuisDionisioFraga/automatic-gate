#include "gpio_control.h"
#include <string.h>

#define MAX_GPIO_COUNT 40

static gpio_cfg_t gpio_table[MAX_GPIO_COUNT];
static int gpio_table_count = 0;

esp_err_t gpio_set_cfg(gpio_cfg_t cfg) {
	for (int i = 0; i < gpio_table_count; i++) {
		if (gpio_table[i].pin == cfg.pin) {
			gpio_table[i] = cfg;
			goto configure_gpio;
		}
	}

	if (gpio_table_count < MAX_GPIO_COUNT) {
		gpio_table[gpio_table_count++] = cfg;
	} else {
		return ESP_ERR_NO_MEM;
	}

configure_gpio:
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << cfg.pin),
		.mode = cfg.mode,
		.pull_up_en = cfg.pullup,
		.pull_down_en = cfg.pulldown,
		.intr_type = GPIO_INTR_DISABLE
	};

	esp_err_t err = gpio_config(&io_conf);
	if (err != ESP_OK) {
		return err;
	}

	if (cfg.mode == GPIO_MODE_OUTPUT) {
		gpio_set_level(cfg.pin, 0);
	}

	return ESP_OK;
}

gpio_cfg_t *gpio_get_cfg(gpio_num_t pin) {
	for (int i = 0; i < gpio_table_count; i++) {
		if (gpio_table[i].pin == pin) {
			return &gpio_table[i];
		}
	}
	return NULL; 
}

esp_err_t gpio_write(gpio_num_t pin, uint32_t state) {
	gpio_cfg_t *cfg = gpio_get_cfg(pin);
	if (!cfg || cfg->mode != GPIO_MODE_OUTPUT) {
		return ESP_ERR_INVALID_STATE;
	}
	gpio_set_level(pin, state);
	return ESP_OK;
}

esp_err_t gpio_toggle(gpio_num_t pin) {
	gpio_cfg_t *cfg = gpio_get_cfg(pin);
	if (!cfg || cfg->mode != GPIO_MODE_OUTPUT) {
		return ESP_ERR_INVALID_STATE;
	}
	int current_state = gpio_get_level(pin);
	gpio_set_level(pin, !current_state);
	return ESP_OK;
}

uint32_t gpio_read(gpio_num_t pin) {
	gpio_cfg_t *cfg = gpio_get_cfg(pin);
	if (!cfg || (cfg->mode != GPIO_MODE_INPUT && cfg->mode != GPIO_MODE_INPUT_OUTPUT)) {
		return -1;
	}
	return gpio_get_level(pin);
}