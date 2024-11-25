/**
 * @file gate.c
 * @author
 *
 * @brief Implementation of the Gate component, providing functionalities to
 * initialize, control, and query the state of a gate.
 *
 * This source file includes the implementation of gate operations, such as
 * opening, stopping, and retrieving its state. It also manages the GPIO pins
 * associated with gate actions.
 *
 * @version 0.1
 * @date 2024-11-20
 *
 * @note `%.*s` allows you to print a substring, where the number of characters
 * printed is determined by a runtime argument (`len` for example).
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "gate.h"

#include <esp_log.h>
#include <string.h>

#include "mqtt5_api.h"

#define OBJECTIVE_STATE_OF_ACTION_ALREADY_ACHIEVED(action, act_state) \
  (((uint8_t)(action)) == ((uint8_t)(act_state)))

static const char *TAG = "GATE";

static gate_t *s_gate_instance = NULL;

/* Forward declaration */
static void gate_init_instance(gate_t *gate_instance, uint8_t gpio_open,
                               uint8_t gpio_close);

/**
 * @brief Handler to MQTT subscription
 *
 */
static void gate_mqtt_handler(char *data, int len)
{
  if (!s_gate_instance)
  {
    ESP_LOGE(TAG, "Gate instance is not initialized");
    return;
  }

  uint8_t action = atoi(data);
  ESP_LOGI(TAG, "Action: %d", action);
  ESP_LOGI(TAG, "State: %d", s_gate_instance->_state);

  if (OBJECTIVE_STATE_OF_ACTION_ALREADY_ACHIEVED(action,
                                                 s_gate_instance->_state))
  {
    ESP_LOGW(TAG, "Gate already is in the objective state");
    return;
  }

  switch (action)
  {
    case GATE_MQTT_OPEN:
    {
      ESP_LOGI(TAG, "Gate in action (opening)");
      s_gate_instance->open(s_gate_instance);

      char topic[MAX_MQTT_TOPIC_LEN];
      snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
               GATE_STATE_TOPIC);

      char gate_state_str[2];
      snprintf(gate_state_str, sizeof(gate_state_str), "%d", GATE_OPENED);

      mqtt5_api_publish(topic, gate_state_str, strlen(gate_state_str));
      break;
    }

    case GATE_MQTT_CLOSE:
    {
      ESP_LOGI(TAG, "Gate in action (closing)");
      s_gate_instance->close(s_gate_instance);

      char topic[MAX_MQTT_TOPIC_LEN];
      snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
               GATE_STATE_TOPIC);

      char gate_state_str[2];
      snprintf(gate_state_str, sizeof(gate_state_str), "%d", GATE_CLOSED);

      mqtt5_api_publish(topic, gate_state_str, strlen(gate_state_str));
      break;
    }

    case GATE_MQTT_STOP:
    {
      ESP_LOGI(TAG, "Gate in action (stopped)");
      s_gate_instance->stop(s_gate_instance);

      char topic[MAX_MQTT_TOPIC_LEN];
      snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
               GATE_STATE_TOPIC);

      char gate_state_str[2];
      snprintf(gate_state_str, sizeof(gate_state_str), "%d", GATE_STOPPED);

      mqtt5_api_publish(topic, gate_state_str, strlen(gate_state_str));
      break;
    }

    default:
      break;
  }
}

// TODO: Implement using GPIO driver
static esp_err_t _configure_gpio(uint8_t gpio_pin)
{
  /**
   *! Implement here
   */

  return ESP_OK;
}

/**
 * @brief Initialize the gate instance.
 *
 * @param self Pointer to the gate instance.
 * @return ESP_OK on success, ESP_FAIL otherwise.
 */
esp_err_t gate_init_impl(gate_t *self, const char *mqtt_subscribe_topic)
{
  if (!self)
    return ESP_FAIL;

  // Initialize the gate instance
  gate_init_instance(self, 0, 0);

  s_gate_instance = self;

  esp_err_t ret;

  mqtt5_api_subscription_t sub = {
    .topic = mqtt_subscribe_topic,
    .callback = &gate_mqtt_handler,
  };
  ret = mqtt5_api_subscribe(&sub);
  if (ret != ESP_OK)
    return ret;

  // Configure the GPIO pins
  ret = _configure_gpio(self->gpio_port_open);
  if (ret != ESP_OK)
    return ret;

  ret = _configure_gpio(self->gpio_port_close);
  if (ret != ESP_OK)
    return ret;

  // Set initial state
  self->_state = GATE_CLOSED;

  ESP_LOGI(TAG, "Gate initialized successfully");
  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_open_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate opening");

  /**
   *! Implement here
   */

  s_gate_instance->_state = GATE_OPENED;

  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_close_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate closing");

  /**
   *! Implement here
   */

  s_gate_instance->_state = GATE_CLOSED;

  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_stop_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate stopped");

  /**
   *! Implement here
   */

  s_gate_instance->_state = GATE_STOPPED;

  return ESP_OK;
}

/**
 * @brief Get the current state of the gate.
 *
 * @param self Pointer to the gate instance.
 * @return The current state of the gate.
 */
static gate_state_t gate_get_state_impl(gate_t *self)
{
  ESP_LOGI(TAG, "Gate state queried: %s",
           self->_state == GATE_OPENED ? "OPENED" : "CLOSED");
  return self->_state;
}

/**
 * @brief Initialize the gate instance.
 *
 * @param gate_instance
 * @param gpio_open
 * @param gpio_close
 */
static void gate_init_instance(gate_t *gate_instance, uint8_t gpio_open,
                               uint8_t gpio_close)
{
  gate_instance->gpio_port_open = gpio_open;
  gate_instance->gpio_port_close = gpio_close;
  gate_instance->init = gate_init_impl;
  gate_instance->open = gate_open_impl;
  gate_instance->close = gate_close_impl;
  gate_instance->stop = gate_stop_impl;
  gate_instance->get_state = gate_get_state_impl;
}
