/**
 * @file gate.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief Gate component
 *
 * @version 0.1
 * @date 2024-11-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef GATE_H
#define GATE_H

#include <esp_err.h>

#define BASE_MQTT_TOPIC "Inatel/C115/2024/Semester/02"
#define GATE_ACTION_TOPIC "gate/action"
#define GATE_STATE_TOPIC "gate/state"
#define GATE_STATE_TOPIC_ANSWER "gate/state/answer"

/**
 * @brief Enum representing the possible states of the gate.
 *
 * @note It's a objective state of each gate action, i.e, the gate receives a
 * action to close then it will be in the state GATE_CLOSED.
 */
typedef enum
{
  GATE_OPENED = 0,  ///< The gate is open.
  GATE_CLOSED,      ///< The gate is closed.
  GATE_STOPPED,     ///< The gate is stopped.
} gate_state_t;

/**
 * @brief Enum representing the possible actions received od MQTT.
 */
typedef enum
{
  GATE_MQTT_OPEN = 0,
  GATE_MQTT_CLOSE,
  GATE_MQTT_STOP,
} gate_mqtt_action_t;

/**
 * @brief Structure representing a gate object.
 */
typedef struct gate
{
  gate_state_t _act_state;  ///< The current state of the gate.
  uint8_t gpio_port_open;   ///< GPIO pin for opening the gate.
  uint8_t gpio_port_close;  ///< GPIO pin for closing the gate.
  char *base_mqtt_topic;    ///< Base MQTT topic for the gate.

  /**
   * @brief Initialize the gate.
   *
   * @return ESP_OK if successful, ESP_FAIL otherwise.
   */
  esp_err_t (*init)(struct gate *self);

  /**
   * @brief Start the action of the gate.
   *
   * @return ESP_OK if successful, ESP_FAIL otherwise.
   */
  esp_err_t (*open)(struct gate *self);

  /**
   * @brief Close the gate.
   *
   * @return ESP_OK if successful, ESP_FAIL otherwise.
   */
  esp_err_t (*close)(struct gate *self);

  /**
   * @brief Stop any ongoing action of the gate.
   *
   * @return ESP_OK if successful, ESP_FAIL otherwise.
   */
  esp_err_t (*stop)(struct gate *self);

  /**
   * @brief Get the current state of the gate.
   *
   * @return The current state of the gate (open or closed).
   */
  gate_state_t (*get_state)(struct gate *self);

} gate_t;

/**
 * @brief Initialize the gate instance.
 *
 * @param self
 * @return ESP_OK on success, ESP_FAIL otherwise.
 */
esp_err_t gate_init_impl(gate_t *self);

#endif  // GATE_H
