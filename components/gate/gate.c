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
static motor_t *s_motor_instance = NULL;

/* Forward declaration */
static void gate_init_instances(gate_t *self);

static void motor_state_to_gate_state()
{
  ESP_LOGI(TAG, "Motor state BEFORE update it: %d",
           s_motor_instance->_act_state);
  motor_action_t motor_state = s_motor_instance->get_state(s_motor_instance);
  ESP_LOGI(TAG, "Motor state AFTER update it: %d",
           s_motor_instance->_act_state);
  switch (motor_state)
  {
    case ACTION_CLOCKWISE_MOTOR:
      s_gate_instance->_act_state = GATE_OPENED;
      break;
    case ACTION_COUNTERCLOCKWISE_MOTOR:
      s_gate_instance->_act_state = GATE_CLOSED;
      break;
    case ACTION_STOP_MOTOR:
      s_gate_instance->_act_state = GATE_STOPPED;
      break;
    default:
      break;
  }
}

static void update_gate_state()
{
  motor_state_to_gate_state();
}

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
  update_gate_state();
  ESP_LOGI(TAG, "State: %d", s_gate_instance->_act_state);

  if (action > (GATE_MQTT_INVALID_ACTION - 1))
  {
    ESP_LOGE(TAG, "Invalid action");
    return;
  }

  if (OBJECTIVE_STATE_OF_ACTION_ALREADY_ACHIEVED(action,
                                                 s_gate_instance->_act_state))
  {
    ESP_LOGW(TAG, "Gate already is in the objective state");

    char topic[MAX_MQTT_TOPIC_LEN];
    snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
             GATE_ACTION_TOPIC_ANSWER);

    char gate_state_str[3];
    snprintf(gate_state_str, sizeof(gate_state_str), "%d", -1);

    mqtt5_api_publish(topic, gate_state_str, strlen(gate_state_str));
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
               GATE_STATE_TOPIC_ANSWER);

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
               GATE_STATE_TOPIC_ANSWER);

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

static void gate_state_mqtt(char *data, int len)
{
  if (!s_gate_instance)
  {
    ESP_LOGE(TAG, "Gate instance is not initialized");
    return;
  }

  update_gate_state();
  ESP_LOGI(TAG, "Gate state queried: %s",
           s_gate_instance->_act_state == GATE_OPENED ? "OPENED" : (s_gate_instance->_act_state == GATE_CLOSED ? "CLOSED" : "STOPPED"));

  char topic[MAX_MQTT_TOPIC_LEN];
  snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
           GATE_STATE_TOPIC_ANSWER);

  char gate_state_str[2];
  snprintf(gate_state_str, sizeof(gate_state_str), "%d",
           s_gate_instance->_act_state);

  mqtt5_api_publish(topic, gate_state_str, strlen(gate_state_str));
}

/**
 * @brief Initialize the gate instance.
 *
 * @param self Pointer to the gate instance.
 * @return ESP_OK on success, ESP_FAIL otherwise.
 */
esp_err_t gate_init_impl(gate_t *self, motor_t *motor)
{
  if (!self)
    return ESP_FAIL;

  // Initialize the gate instance
  gate_init_instances(self);

  // Initialize the motor
  s_motor_instance = motor;
  motor_init(motor);
  motor_start_task();

  // Subscribe to MQTT topics
  esp_err_t ret;

  mqtt5_api_subscription_t sub_gate_action = {
    .callback = &gate_mqtt_handler,
  };
  snprintf(sub_gate_action.topic, MAX_MQTT_TOPIC_LEN, "%s/%s", BASE_MQTT_TOPIC,
           GATE_ACTION_TOPIC);

  ret = mqtt5_api_subscribe(&sub_gate_action);
  if (ret != ESP_OK)
    return ret;

  mqtt5_api_subscription_t sub_gate_state = {
    .callback = &gate_state_mqtt,
  };
  snprintf(sub_gate_state.topic, MAX_MQTT_TOPIC_LEN, "%s/%s", BASE_MQTT_TOPIC,
           GATE_STATE_TOPIC);

  ret = mqtt5_api_subscribe(&sub_gate_state);
  if (ret != ESP_OK)
    return ret;

  // Set initial state
  self->_act_state = GATE_CLOSED;

  ESP_LOGI(TAG, "Gate initialized successfully");
  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_open_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate opening");

  s_motor_instance->in_action(ACTION_CLOCKWISE_MOTOR);

  s_gate_instance->_act_state = GATE_OPENED;

  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_close_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate closing");

  s_motor_instance->in_action(ACTION_COUNTERCLOCKWISE_MOTOR);

  s_gate_instance->_act_state = GATE_CLOSED;

  return ESP_OK;
}

// TODO: Implement using GPIO driver
static esp_err_t gate_stop_impl(gate_t *self)
{
  if (!self)
    return ESP_FAIL;

  ESP_LOGI(TAG, "Gate stopped");

  s_motor_instance->in_action(ACTION_STOP_MOTOR);

  s_gate_instance->_act_state = GATE_STOPPED;

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
           self->_act_state == GATE_OPENED ? "OPENED" : "CLOSED");
  return self->_act_state;
}

/**
 * @brief Initialize the gate instance.
 *
 * @param gate_instance
 */
static void gate_init_instances(gate_t *self)
{
  s_gate_instance = self;

  self->open = gate_open_impl;
  self->close = gate_close_impl;
  self->stop = gate_stop_impl;
  self->get_state = gate_get_state_impl;
}
