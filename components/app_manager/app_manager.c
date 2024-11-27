/**
 * @file app_manager.c
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief Application Manager implementation
 *
 * @version 0.1
 * @date 2024-11-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "app_manager.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#include "gate.h"
#include "mqtt5_secrets.h"
#include "wifi_secrets.h"

#define WIFI_CONNECTED_BIT BIT0
#define MQTT_CONNECTED_BIT BIT1

#define TOPIC_TO_FIRST_MESSAGE "first_message"

#define FREERTOS_ERR_CHECK(x)                                       \
  do                                                                \
  {                                                                 \
    if ((x) != pdTRUE)                                              \
    {                                                               \
      ESP_LOGE(TAG, "FreeRTOS error at %s:%d", __FILE__, __LINE__); \
      abort();                                                      \
    }                                                               \
  } while (0)

static const char *TAG = "APP MANAGER";

static TaskHandle_t wifi_task_handle = NULL;
static TaskHandle_t mqtt5_task_handle = NULL;
static TaskHandle_t gate_task_handle = NULL;

static EventGroupHandle_t wifi_connected_bit = NULL;
static EventGroupHandle_t mqtt5_connected_bit = NULL;

/**
 * @brief Manage the Wi-Fi connection.
 *
 * This task configures the Wi-Fi connection and sets the event bit when
 * connected. It runs indefinitely with a delay but does not perform any
 * other actions. In future versions, it may manages Wi-Fi connection
 *
 * @param pvParameters Parameters passed to the task (not used).
 */
static void wifi_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Starting Wi-Fi Task...");

  if (wifi_api_configure(WIFI_SSID, WIFI_PASSWORD) == ESP_OK)
  {
    ESP_LOGI(TAG, "Wi-Fi configured.");
    xEventGroupSetBits(wifi_connected_bit, WIFI_CONNECTED_BIT);
  }

  while (1)
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Manage the MQTT5 connection.
 *
 * This task waits for the Wi-Fi connection to be established and then starts
 * the MQTT5 client. It publishes a message and subscribes to a topic.
 * It runs indefinitely with a delay but does not perform any other actions.
 * In future versions, it may manage the MQTT5 connection.
 *
 * @param pvParameters Parameters passed to the task (not used).'
 */
static void mqtt5_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Starting MQTT5 Task...");

  // Wait for Wi-Fi to connect
  EventBits_t bits = xEventGroupWaitBits(wifi_connected_bit, WIFI_CONNECTED_BIT,
                                         pdTRUE, pdFALSE, portMAX_DELAY);
  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI(TAG, "Wi-Fi connected, starting MQTT5...");
    mqtt5_api_start(MQTT5_URL, MQTT5_USERNAME, MQTT5_PASSWORD, MQTT5_PORT);

    const char *msg = "MQTT5 connected!";

    char topic[MAX_MQTT_TOPIC_LEN];
    snprintf(topic, sizeof(topic), "%s/%s", BASE_MQTT_TOPIC,
             TOPIC_TO_FIRST_MESSAGE);

    esp_err_t ret;
    ret = mqtt5_api_publish(topic, msg, strlen(msg));
    if (ret != ESP_OK)
      return;

    xEventGroupSetBits(mqtt5_connected_bit, MQTT_CONNECTED_BIT);
  }

  while (1)
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

static void gate_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Starting gate task...");

  // Wait for Wi-Fi to connect
  EventBits_t bits = xEventGroupWaitBits(
    mqtt5_connected_bit, MQTT_CONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

  if (bits & MQTT_CONNECTED_BIT)
  {
    ESP_LOGI(TAG, "MQTT5 connected, starting gate...");

    gate_t gate;
    gate_init_impl(&gate);
  }

  while (1)
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void application_manager_init()
{
  wifi_connected_bit = xEventGroupCreate();
  xEventGroupClearBits(wifi_connected_bit, WIFI_CONNECTED_BIT);

  mqtt5_connected_bit = xEventGroupCreate();
  xEventGroupClearBits(mqtt5_connected_bit, MQTT_CONNECTED_BIT);

  ESP_LOGI(TAG, "Initializing Application Manager...");
  FREERTOS_ERR_CHECK(xTaskCreate(&wifi_task, "WiFi Task", 4096, NULL,
                                 tskIDLE_PRIORITY + 1, &wifi_task_handle));

  FREERTOS_ERR_CHECK(xTaskCreate(&mqtt5_task, "MQTT5 Task", 4096, NULL,
                                 tskIDLE_PRIORITY + 1, &mqtt5_task_handle));

  FREERTOS_ERR_CHECK(xTaskCreate(&gate_task, "Gate Task", 4096, NULL,
                                 tskIDLE_PRIORITY + 1, &gate_task_handle));

  ESP_LOGI(TAG, "Application Manager initialized.");
}