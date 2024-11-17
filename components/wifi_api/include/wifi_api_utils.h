/**
 * @file wifi_api_utils.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief Wi-Fi API utility functions header file.
 *
 * @version 0.1
 * @date 2024-11-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <esp_log.h>
#include <esp_wifi.h>
#include <stdint.h>

/**
 * @brief Print the authentication mode of the Wi-Fi network.
 *
 * This function prints the authentication mode of the Wi-Fi network based on
 * the `authmode` parameter.
 *
 * @param authmode The authentication mode of the Wi-Fi network.
 */
static void print_auth_mode(uint8_t authmode)
{
  switch (authmode)
  {
    case WIFI_AUTH_OPEN:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_OPEN");
      break;
    case WIFI_AUTH_OWE:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_OWE");
      break;
    case WIFI_AUTH_WEP:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WEP");
      break;
    case WIFI_AUTH_WPA_PSK:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA_PSK");
      break;
    case WIFI_AUTH_WPA2_PSK:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA2_PSK");
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
      break;
    case WIFI_AUTH_WPA3_PSK:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
      break;
    case WIFI_AUTH_WPA3_ENT_192:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_WPA3_ENT_192");
      break;
    default:
      ESP_LOGI("WIFI API UTILS", "Authmode \tWIFI_AUTH_UNKNOWN");
      break;
  }
}
