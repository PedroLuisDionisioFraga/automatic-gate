/**
 * @file motor.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief
 *
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "gpio_drivers.h"

/**
 * @brief Enumeration of motor states.
 *
 * @note It's a objective state of each motor action, i.e, the motor receives a
 * action to stop (MOTOR_STOP) then it will be in the state
 * (STATE_MOTOR_STOPPED).
 */
typedef enum
{
  STATE_MOTOR_STOPPED = 0,         ///< The motor is stopped.
  STATE_MOTOR_IN_CLOCKWISE,        ///< Opening the gate.
  STATE_MOTOR_IN_COUNTERCLOCKWISE  ///< Closing the gate.
} motor_state_t;

typedef enum
{
  ACTION_STOP_MOTOR = 0,          ///< Stopped the motor.
  ACTION_CLOCKWISE_MOTOR,         ///< Opened the gate.
  ACTION_COUNTERCLOCKWISE_MOTOR,  ///< Closed the gate.
} motor_action_t;

typedef struct motor
{
  gpio_pinout_t gpio_pinout;  ///< GPIO pin for the motor.
  motor_state_t _act_state;   ///< Current state of the motor.
  motor_state_t _last_state;  ///< Last state of the motor.
  motor_action_t _action;     ///< Current action of the motor.

  /**
   * @brief Update the state of the motor.
   *
   * @param self Pointer to the motor object.
   */
  void (*update_state)(struct motor *self, motor_state_t state);

  /**
   * @brief Open the gate.
   *
   * @param self Pointer to the motor object.
   */
  void (*open)(struct motor *self);

  /**
   * @brief Close the gate.
   *
   * @param self Pointer to the motor object.
   */
  void (*close)(struct motor *self);

  /**
   * @brief Stop the motor.
   *
   * @param self Pointer to the motor object.
   */
  void (*stop)(struct motor *self);

  /**
   * @brief Toggle the state of the motor.
   *
   * @param self Pointer to the motor object.
   */
  void (*toggle)(struct motor *self);

} motor_t;

/**
 * @brief Initializes the motor.
 */
void motor_init(motor_t *self);

/**
 * @brief Start the motor task.
 */
void motor_start_task();

#endif  // MOTOR_H
