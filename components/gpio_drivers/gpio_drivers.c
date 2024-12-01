/**
 * @file gpio_drivers.c
 * @author Marcos Henrique Silveira Barbosa
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "gpio_drivers.h"

#include <esp_log.h>
#include <stdbool.h>
#include <string.h>

#define GPIO_ISR_SERVICE_DEFAULT_FLAGS 0

static const char *TAG = "GPIO";

static bool isr_service_installed = false;

static gpio_t *s_gpio_instance = NULL;

esp_err_t gpio_set_config_output(gpio_pinout_t pin)
{
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << pin),
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_up_en = GPIO_PULLUP_DISABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .intr_type = GPIO_INTR_DISABLE};

  ESP_ERROR_CHECK(gpio_config(&io_conf));

  ESP_LOGI(TAG, "Configured pin %d as output", pin);

  return ESP_OK;
}

esp_err_t gpio_set_config_input(gpio_pinout_t pin, void isr_handler(void *),
                                void *isr_handler_arg)
{
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << (uint8_t)pin),
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = GPIO_PULLUP_ENABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .intr_type = GPIO_INTR_NEGEDGE};

  ESP_ERROR_CHECK(gpio_config(&io_conf));
  ESP_LOGI(TAG, "Configured pin %d as input", pin);

  if (isr_handler == NULL)
    return ESP_OK;

  ESP_ERROR_CHECK(gpio_isr_handler_add(pin, isr_handler, isr_handler_arg));
  ESP_LOGI(TAG, "Configured ISR handler for pin %d", pin);

  return ESP_OK;
}

esp_err_t gpio_write(gpio_t *self, gpio_state_t state)
{
  gpio_set_level(self->pin, (uint32_t)state);
  return ESP_OK;
}

gpio_state_t gpio_read(gpio_t *self)
{
  return gpio_get_level(self->pin);
}

void gpio_toggle(gpio_t *self)
{
  gpio_state_t state = gpio_read(self);
  gpio_write(self, state == GPIO_STATE_LOW ? GPIO_STATE_HIGH : GPIO_STATE_LOW);
}

// TODO: Finish GPIO driver implementation
void gpio_init_impl(gpio_t *self)
{
  s_gpio_instance = self;
  s_gpio_instance->get_state = &gpio_read;
  s_gpio_instance->set_state = &gpio_write;
  //s_gpio_instance->toggle = &gpio_toggle;

  switch (s_gpio_instance->_mode)
  {
    case GPIO_MODE_INPUT:
    {
      gpio_set_config_input(s_gpio_instance->pin, s_gpio_instance->isr_handler,
                            NULL);
      break;
    }
    case GPIO_MODE_OUTPUT:
    {
      gpio_set_config_output(s_gpio_instance->pin);
      gpio_write(s_gpio_instance, s_gpio_instance->_act_state);
      break;
    }
    default:
    {
      ESP_LOGE(TAG, "Invalid GPIO mode");
      break;
    }
  }

  if (!isr_service_installed)
  {
    ESP_ERROR_CHECK(gpio_install_isr_service(GPIO_ISR_SERVICE_DEFAULT_FLAGS));
    isr_service_installed = true;
  }
  else
    ESP_LOGI(TAG, "ISR service already installed");
}

esp_err_t gpio_disable_isr(gpio_t *self)
{
  return gpio_intr_disable(self->pin);
}

esp_err_t gpio_enable_isr(gpio_t *self)
{
  return gpio_intr_enable(self->pin);
}
