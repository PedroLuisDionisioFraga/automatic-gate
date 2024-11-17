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

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "app_manager.h"

#include "wifi_secrets.h"

static const char *TAG = "APP MANAGER";
static TaskHandle_t wifi_task_handle;

void wifi_task(void *pvParameters)
{
  ESP_LOGI(TAG, "Starting Wi-Fi Task...");

  wifi_api_configure(WIFI_SSID, WIFI_PASSWORD);
}

void application_manager_init()
{
  BaseType_t result;

  ESP_LOGI(TAG, "Initializing Application Manager...");
  result = xTaskCreate(&wifi_task, "WiFi Task", 4096, NULL,
                       tskIDLE_PRIORITY + 1, &wifi_task_handle);
  if (result != pdPASS)
    return;

  ESP_LOGI(TAG, "Application Manager initialized.");
}
