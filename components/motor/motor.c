/**
 * @file motor.c
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "motor.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "gpio_drivers.h"

#define MOTOR_CONTROL_PIN D14

#define OPEN_ENDLINE_SENSOR_PIN D23
#define CLOSE_ENDLINE_SENSOR_PIN D22

#define LED_OPENED D27
#define LED_CLOSED D26
#define LED_STOPPED D25

#define MAX_QUEUE_SIZE 10

static const char *TAG = "MOTOR";

static QueueHandle_t s_motor_event_queue;

volatile int motor_interrupt_count = 0;

static motor_t *s_motor_instance = NULL;

static TimerHandle_t s_motor_timer_enable_isr = NULL;

static void motor_control(void *arg);
static void motor_opened(void *arg);
static void motor_closed(void *arg);

static gpio_t s_motor_control = {
  .pin = MOTOR_CONTROL_PIN,
  ._mode = GPIO_MODE_INPUT,
  .isr_handler = motor_control,
  .isr_handler_arg = (void *)MOTOR_CONTROL_PIN,
};
static gpio_t s_open_endline_sensor = {
  .pin = OPEN_ENDLINE_SENSOR_PIN,
  ._mode = GPIO_MODE_INPUT,
  .isr_handler = motor_opened,
  .isr_handler_arg = (void *)OPEN_ENDLINE_SENSOR_PIN,
};
static gpio_t s_close_endline_sensor = {
  .pin = CLOSE_ENDLINE_SENSOR_PIN,
  ._mode = GPIO_MODE_INPUT,
  .isr_handler = motor_closed,
  .isr_handler_arg = (void *)CLOSE_ENDLINE_SENSOR_PIN,
};

static gpio_t s_led_closing = {
  .pin = LED_CLOSED,
  ._mode = GPIO_MODE_OUTPUT,
  ._act_state = GPIO_STATE_LOW,
  .isr_handler = NULL,
  .isr_handler_arg = NULL,
};
static gpio_t s_led_opening = {
  .pin = LED_OPENED,
  ._mode = GPIO_MODE_OUTPUT,
  ._act_state = GPIO_STATE_LOW,
  .isr_handler = NULL,
  .isr_handler_arg = NULL,
};
static gpio_t s_led_stopping = {
  .pin = LED_STOPPED,
  ._mode = GPIO_MODE_OUTPUT,
  ._act_state = GPIO_STATE_HIGH,
  .isr_handler = NULL,
  .isr_handler_arg = NULL,
};

static void update_state(motor_t *self, motor_state_t state)
{
  self->_last_state = self->_act_state;
  // Convert the motor action to the motor state
  self->_act_state = state;
}

//* Callback function of motor INTERRUPT
static void motor_control(void *arg)
{
  gpio_pinout_t pin = (gpio_pinout_t)arg;

  motor_interrupt_count++;

  motor_state_t next_state;
  switch (s_motor_instance->_act_state)
  {
    case STATE_MOTOR_STOPPED:
    {
      next_state = (s_motor_instance->_last_state == STATE_MOTOR_IN_CLOCKWISE)
                     ? STATE_MOTOR_IN_COUNTERCLOCKWISE
                     : STATE_MOTOR_IN_CLOCKWISE;
      break;
    }
    case STATE_MOTOR_IN_COUNTERCLOCKWISE:
    case STATE_MOTOR_IN_CLOCKWISE:
    {
      next_state = STATE_MOTOR_STOPPED;
      break;
    }
    default:
    {
      return;
    }
  }

  s_motor_instance->_action = (motor_action_t)next_state;
  s_motor_instance->update_state(s_motor_instance, next_state);

  gpio_disable_isr(&s_motor_control);
  xTimerStart(s_motor_timer_enable_isr, 0);

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(s_motor_event_queue, &s_motor_instance->_action,
                    &xHigherPriorityTaskWoken);
}

// Helper function to set LED states
static esp_err_t set_led_states(gpio_state_t opening, gpio_state_t closing,
                                gpio_state_t stopping)
{
  esp_err_t ret;

  ret = s_led_opening.set_state(&s_led_opening, opening);
  if (ret != ESP_OK)
    return ret;

  ret = s_led_closing.set_state(&s_led_closing, closing);
  if (ret != ESP_OK)
    return ret;

  ret = s_led_stopping.set_state(&s_led_stopping, stopping);
  if (ret != ESP_OK)
    return ret;

  return ESP_OK;
}

//* (Motor task) to update the LED states based on received QUEUE
static void motor_task(void *pvParameters)
{
  motor_action_t rcv_action;
  while (1)
  {
    if (xQueueReceive(s_motor_event_queue, &rcv_action, portMAX_DELAY) ==
        pdTRUE)
    {
      esp_err_t ret;

      ESP_LOGI(TAG, "Motor receive action: %s",
               (rcv_action == ACTION_STOP_MOTOR) ? "ACTION_STOP_MOTOR"
               : (rcv_action == ACTION_CLOCKWISE_MOTOR)
                 ? "ACTION_CLOCKWISE_MOTOR"
                 : "ACTION_COUNTERCLOCKWISE_MOTOR");

      switch (rcv_action)
      {
        case ACTION_STOP_MOTOR:
        {
          ret = set_led_states(GPIO_STATE_LOW, GPIO_STATE_LOW, GPIO_STATE_HIGH);

          gpio_disable_isr(&s_open_endline_sensor);
          gpio_disable_isr(&s_close_endline_sensor);

          break;
        }
        case ACTION_CLOCKWISE_MOTOR:
        {
          ret = set_led_states(GPIO_STATE_HIGH, GPIO_STATE_LOW, GPIO_STATE_LOW);

          gpio_enable_isr(&s_open_endline_sensor);
          gpio_disable_isr(&s_close_endline_sensor);

          break;
        }
        case ACTION_COUNTERCLOCKWISE_MOTOR:
        {
          ret = set_led_states(GPIO_STATE_LOW, GPIO_STATE_HIGH, GPIO_STATE_LOW);

          gpio_enable_isr(&s_close_endline_sensor);
          gpio_disable_isr(&s_open_endline_sensor);

          break;
        }
        default:
          return;
      }
    }
  }
}

static void motor_opened(void *arg)
{
  motor_interrupt_count++;
  s_led_opening.set_state(&s_led_opening, GPIO_STATE_LOW);
  s_led_stopping.set_state(&s_led_stopping, GPIO_STATE_HIGH);

  gpio_disable_isr(&s_open_endline_sensor);
  gpio_disable_isr(&s_close_endline_sensor);

  s_motor_instance->update_state(s_motor_instance, STATE_MOTOR_STOPPED);
}

static void motor_closed(void *arg)
{
  motor_interrupt_count++;
  s_led_closing.set_state(&s_led_closing, GPIO_STATE_LOW);
  s_led_stopping.set_state(&s_led_stopping, GPIO_STATE_HIGH);

  gpio_disable_isr(&s_open_endline_sensor);
  gpio_disable_isr(&s_close_endline_sensor);

  s_motor_instance->update_state(s_motor_instance, STATE_MOTOR_STOPPED);
}

static void motor_enable_isr(TimerHandle_t xTimer)
{
  gpio_enable_isr(&s_motor_control);
}

void motor_init(motor_t *self)
{
  s_motor_instance = self;
  s_motor_instance->_act_state = STATE_MOTOR_STOPPED;
  s_motor_instance->_last_state = STATE_MOTOR_IN_COUNTERCLOCKWISE;
  s_motor_instance->update_state = &update_state;
  // TODO: Implement the toggle function

  s_motor_event_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(motor_state_t));

  // Initialize the output GPIOs
  gpio_init_impl(&s_led_closing);
  gpio_init_impl(&s_led_opening);
  gpio_init_impl(&s_led_stopping);

  // Initialize the input GPIOs
  gpio_init_impl(&s_motor_control);
  gpio_init_impl(&s_open_endline_sensor);
  gpio_init_impl(&s_close_endline_sensor);

  ESP_LOGI(TAG, "Motor initialized");

  xTaskCreate(motor_task, "motor_task", 2048, NULL, 10, NULL);

  // Create a timer to enable the ISR service
  s_motor_timer_enable_isr =
    xTimerCreate("Motor Timer to Enable ISR", pdMS_TO_TICKS(2000), pdTRUE, NULL,
                 motor_enable_isr);
}
