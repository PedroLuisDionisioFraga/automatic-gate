/**
 * @file gpio_drivers.h
 * @brief GPIO Drivers for configuring and controlling GPIOs.
 * @author Marcos Henrique Silveira Barbosa
 *
 * This header provides function declarations for initializing GPIOs, setting
 * configurations, reading, writing, and toggling GPIO states.
 *
 * @version 0.1
 * @date 2024-11-26
 */

#ifndef GPIO_DRIVERS_H
#define GPIO_DRIVERS_H

#include <driver/gpio.h>
#include <esp_err.h>

/**
 * @brief Enumeration of GPIO pin definitions.
 *
 * This enumeration maps logical names to the corresponding GPIO numbers
 * of the ESP32 microcontroller.
 */
typedef enum
{
  // Upper Pins
  D13 = GPIO_NUM_13,
  D12 = GPIO_NUM_12,
  D14 = GPIO_NUM_14,
  D27 = GPIO_NUM_27,
  D26 = GPIO_NUM_26,
  D25 = GPIO_NUM_25,
  D33 = GPIO_NUM_33,
  D32 = GPIO_NUM_32,
  D35 = GPIO_NUM_35,
  D34 = GPIO_NUM_34,

  // Lower Pins
  D15 = GPIO_NUM_15,
  D4 = GPIO_NUM_4,
  D5 = GPIO_NUM_5,
  D18 = GPIO_NUM_18,
  D19 = GPIO_NUM_19,
  D21 = GPIO_NUM_21,
  D22 = GPIO_NUM_22,
  D23 = GPIO_NUM_23,

  // Built-in LED
  BUILTIN_LED = GPIO_NUM_2,

  // UART Pins
  RX0 = GPIO_NUM_3,
  TX0 = GPIO_NUM_1,
  RX2 = GPIO_NUM_16,
  TX2 = GPIO_NUM_17,

  // I2C Pins
  SDA = GPIO_NUM_21,
  SCL = GPIO_NUM_22,

  // VSPI Pins
  VSPI_MISO = GPIO_NUM_19,
  VSPI_MOSI = GPIO_NUM_23,
  VSPI_CLK = GPIO_NUM_18,
  VSPI_CS = GPIO_NUM_5,

  // HSPI Pins
  HSPI_MISO = GPIO_NUM_12,
  HSPI_MOSI = GPIO_NUM_13,
  HSPI_CLK = GPIO_NUM_14,
  HSPI_CS = GPIO_NUM_15,

  // SPI Pins
  SPI_MOSI = GPIO_NUM_23,
  SPI_MISO = GPIO_NUM_19,
  SPI_SCK = GPIO_NUM_18,
  SPI_CS = GPIO_NUM_5,

  // ADC Pins
  ADC1_CH0 = GPIO_NUM_36,
  ADC1_CH3 = GPIO_NUM_39,
  ADC1_CH4 = GPIO_NUM_32,
  ADC1_CH5 = GPIO_NUM_33,
  ADC1_CH6 = GPIO_NUM_34,
  ADC1_CH7 = GPIO_NUM_35,
  ADC2_CH0 = GPIO_NUM_4,
  ADC2_CH2 = GPIO_NUM_2,
  ADC2_CH3 = GPIO_NUM_15,
  ADC2_CH4 = GPIO_NUM_13,
  ADC2_CH5 = GPIO_NUM_12,
  ADC2_CH6 = GPIO_NUM_14,
  ADC2_CH7 = GPIO_NUM_27,
  ADC2_CH8 = GPIO_NUM_25,
  ADC2_CH9 = GPIO_NUM_26,

  // DAC Pins
  DAC1 = GPIO_NUM_25,
  DAC2 = GPIO_NUM_26,

  // Touch Sensor Pins
  TOUCH0 = GPIO_NUM_4,
  TOUCH2 = GPIO_NUM_2,
  TOUCH3 = GPIO_NUM_15,
  TOUCH4 = GPIO_NUM_13,
  TOUCH5 = GPIO_NUM_12,
  TOUCH6 = GPIO_NUM_14,
  TOUCH7 = GPIO_NUM_27,
  TOUCH8 = GPIO_NUM_33,
  TOUCH9 = GPIO_NUM_32,

  // RTC GPIOs
  RTC_0 = GPIO_NUM_36,
  RTC_3 = GPIO_NUM_39,
  RTC_4 = GPIO_NUM_34,
  RTC_5 = GPIO_NUM_35,
  RTC_6 = GPIO_NUM_25,
  RTC_7 = GPIO_NUM_26,
  RTC_8 = GPIO_NUM_33,
  RTC_9 = GPIO_NUM_32,
  RTC_10 = GPIO_NUM_4,
  RTC_12 = GPIO_NUM_2,
  RTC_13 = GPIO_NUM_15,
  RTC_14 = GPIO_NUM_13,
  RTC_15 = GPIO_NUM_12,
  RTC_16 = GPIO_NUM_14,
  RTC_17 = GPIO_NUM_27,

  // Strapping Pins
  STRAP1 = GPIO_NUM_12,
  STRAP2 = GPIO_NUM_5,
  STRAP3 = GPIO_NUM_2,
  STRAP4 = GPIO_NUM_15,

  // Disabled GPIO
  DISABLE = GPIO_NUM_NC,
} gpio_pinout_t;

/**
 * @brief Enumeration of GPIO states.
 */
typedef enum
{
  GPIO_STATE_LOW = 0,
  GPIO_STATE_HIGH = 1,
} gpio_state_t;

/**
 * @brief Structure representing a GPIO object.
 */
typedef struct gpio
{
  struct gpio *next;       /**< Pointer to the next GPIO object in the list */
  gpio_pinout_t pin;       /**< GPIO pin number */
  gpio_state_t _act_state; /**< Current state of the GPIO pin */
  gpio_config_t _config;   /**< Configuration of the GPIO pin */
  gpio_mode_t _mode;       /**< Mode of the GPIO pin */

  void (*isr_handler)(void *); /**< ISR handler function */
  void *isr_handler_arg;       /**< Argument to the ISR handler function */

  /**
   * @brief Initialize the GPIO object.
   *
   * @param self Pointer to the GPIO object.
   * @return
   * - **ESP_OK** on success
   * - **ESP_ERR_INVALID_ARG** if the parameters are invalid
   * - **ESP_FAIL** on other errors
   */
  esp_err_t (*init)(struct gpio *self);

  /**
   * @brief Set the state of the GPIO pin.
   *
   * @param self Pointer to the GPIO object.
   * @param state Desired state of the GPIO pin.
   * @return
   * - **ESP_OK** on success
   * - **ESP_ERR_INVALID_ARG** if the parameters are invalid
   * - **ESP_FAIL** on other errors
   */
  esp_err_t (*set_state)(struct gpio *self, gpio_state_t state);

  /**
   * @brief Get the current state of the GPIO pin.
   *
   * @param self Pointer to the GPIO object.
   * @return
   * - Current state of the GPIO pin
   */
  gpio_state_t (*get_state)(struct gpio *self);

  /**
   * @brief Toggle the state of the GPIO pin.
   *
   * @param self Pointer to the GPIO object.
   * @return
   * - **ESP_OK** on success
   * - **ESP_ERR_INVALID_ARG** if the parameters are invalid
   * - **ESP_FAIL** on other errors
   */
  esp_err_t (*toggle)(struct gpio *self);
} gpio_t;

/**
 * @brief Initialize the GPIO implementation.
 *
 * @param self Pointer to the GPIO object.
 */
void gpio_init_impl(gpio_t *self);

/**
 * @brief Disable the ISR for the specified GPIO.
 *
 * @param self Pointer to the GPIO object.
 * @return
 * - **ESP_OK** on success
 * - **ESP_ERR_INVALID_ARG** if the parameters are invalid
 * - **ESP_FAIL** on other errors
 */
esp_err_t gpio_disable_isr(gpio_t *self);

/**
 * @brief Enable the ISR for the specified GPIO.
 *
 * @param self Pointer to the GPIO object.
 * @return
 * - **ESP_OK** on success
 * - **ESP_ERR_INVALID_ARG** if the parameters are invalid
 * - **ESP_FAIL** on other errors
 */
esp_err_t gpio_enable_isr(gpio_t *self);

#endif  // GPIO_DRIVERS_H