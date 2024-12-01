#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "app_manager.h"

static const char *TAG = "MAIN";

void manage_logs()
{
  esp_log_level_set("*", ESP_LOG_NONE);
  esp_log_level_set(TAG, ESP_LOG_INFO);
  esp_log_level_set("APP MANAGER", ESP_LOG_INFO);
  esp_log_level_set("GPIO", ESP_LOG_INFO);
  esp_log_level_set("MOTOR", ESP_LOG_INFO);
}

void app_main(void)
{
  //manage_logs();

  ESP_LOGI(TAG, "Initializing Application Manager...");
  application_manager_init();
  while (1)
  {
    ESP_LOGI(TAG, "Main loop...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
