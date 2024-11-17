/**
 * @file mqtt5_properties.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief Properties to be used in MQTT 5
 *
 * @version 0.1
 * @date 2024-11-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MQTT5_PROPERTIES_H
#define MQTT5_PROPERTIES_H

#include <mqtt_client.h>

// TODO: Study how to use this property
static esp_mqtt5_user_property_item_t user_property_arr[] = {
  {"board", "esp32"}, {"u", "user"}, {"p", "password"}};

// TODO: Study how to use this property
static esp_mqtt5_publish_property_config_t publish_property = {
  .payload_format_indicator = 1,
  .message_expiry_interval = 1000,
  .topic_alias = 0,
  .response_topic = "c115/publish123",
  .correlation_data = "123456",
  .correlation_data_len = 6,
};

// TODO: Study how to use this property
static esp_mqtt5_subscribe_property_config_t subscribe_property = {
  .no_local_flag = false,
  .retain_as_published_flag = false,
  .retain_handle = 0,
  .is_share_subscribe = true,
  .share_name = "group1",
};

// TODO: Study how to use this property
static esp_mqtt5_subscribe_property_config_t subscribe1_property = {
  .no_local_flag = true,
  .retain_as_published_flag = false,
  .retain_handle = 0,
};

// TODO: Study how to use this property
static esp_mqtt5_unsubscribe_property_config_t unsubscribe_property = {
  .is_share_subscribe = true,
  .share_name = "group1",
};

// TODO: Study how to use this property
static esp_mqtt5_disconnect_property_config_t disconnect_property = {
  .session_expiry_interval = 60,
  .disconnect_reason = 0,
};

/**
 * @brief Print MQTT 5.0 user properties.
 *
 * This function prints the key-value pairs of MQTT 5.0 user properties.
 *
 * @param user_property The handle to the user property.
 */
void print_user_property(mqtt5_user_property_handle_t user_property)
{
  if (user_property)
  {
    uint8_t count = esp_mqtt5_client_get_user_property_count(user_property);
    if (count)
    {
      esp_mqtt5_user_property_item_t *item =
        malloc(count * sizeof(esp_mqtt5_user_property_item_t));
      if (esp_mqtt5_client_get_user_property(user_property, item, &count) ==
          ESP_OK)
      {
        for (int i = 0; i < count; i++)
        {
          esp_mqtt5_user_property_item_t *t = &item[i];
          ESP_LOGI("MQTT5 API UTILS", "key is %s, value is %s", t->key, t->value);
          free((char *)t->key);
          free((char *)t->value);
        }
      }
      free(item);
    }
  }
}

#endif // MQTT5_PROPERTIES_H
