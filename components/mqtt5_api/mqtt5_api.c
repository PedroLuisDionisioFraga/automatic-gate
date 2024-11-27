/**
 * @file mqtt5_api.c
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief MQTT 5 API implementation
 *
 * @version 0.1
 * @date 2024-11-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mqtt5_api.h"

#include <esp_event.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <string.h>

#include "mqtt5_properties.h"

#define MAX_TOPICS_SUBSCRIBED 10

static const char *TAG = "MQTT5 API";
static esp_mqtt_client_handle_t client = NULL;

static mqtt5_api_subscription_t s_subscriptions[MAX_TOPICS_SUBSCRIBED];

static void _add_mqtt5_subscription(mqtt5_api_subscription_t *subscription)
{
  for (int i = 0; i < MAX_TOPICS_SUBSCRIBED; i++)
  {
    if (s_subscriptions[i].topic[0] != '\0')
      continue;

    strncpy(s_subscriptions[i].topic, subscription->topic,
            MAX_MQTT_TOPIC_LEN - 1);
    s_subscriptions[i].topic[MAX_MQTT_TOPIC_LEN - 1] = '\0';

    s_subscriptions[i].callback = subscription->callback;

    ESP_LOGW(TAG, "Subscription added to index %d", i);
    ESP_LOGW(TAG, "Topic: %s", s_subscriptions[i].topic);
    break;
  }
}
static inline bool _is_same_subscription(mqtt5_api_subscription_t *s1,
                                         mqtt5_api_subscription_t *s2)
{
  return (strcmp(s1->topic, s2->topic) == 0) && (s1->callback == s2->callback);
}

/**
 * @brief Event handler for MQTT events.
 *
 * This function handles various MQTT events such as connection, disconnection,
 * subscription, unsubscription, message publication, message reception, and
 * errors.
 *
 * @param handler_args User-defined argument (not used).
 * @param base Event base.
 * @param event_id Event ID.
 * @param event_data Event-specific data.
 */
static void mqtt5_api_event_handler(void *handler_args, esp_event_base_t base,
                                    int32_t event_id, void *event_data)
{
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;
  switch ((esp_mqtt_event_id_t)event_id)
  {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
      // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
      // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      break;

    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      break;

    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
      break;

    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
      break;

    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;

    case MQTT_EVENT_DATA:
      ESP_LOGI(TAG, "MQTT_EVENT_DATA");
      printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
      printf("DATA=%.*s\r\n", event->data_len, event->data);
      for (int i = 0; i < MAX_TOPICS_SUBSCRIBED; i++)
      {
        if (s_subscriptions[i].topic[0] == '\0' ||
            s_subscriptions[i].callback == NULL)
          continue;

        ESP_LOGW(TAG, "Comparing '%.*s' with '%s'", event->topic_len,
                 event->topic, s_subscriptions[i].topic);

        if (strncmp(event->topic, s_subscriptions[i].topic, event->topic_len) ==
              0 &&
            strlen(s_subscriptions[i].topic) == event->topic_len)
        {
          s_subscriptions[i].callback(event->data, event->data_len);
          break;
        }
      }
      break;

    case MQTT_EVENT_ERROR:
      ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
      if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS)
      {
        ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x",
                 event->error_handle->esp_tls_last_esp_err);
        ESP_LOGI(TAG, "Last tls stack error number: 0x%x",
                 event->error_handle->esp_tls_stack_err);
        ESP_LOGI(TAG, "Last captured errno : %d (%s)",
                 event->error_handle->esp_transport_sock_errno,
                 strerror(event->error_handle->esp_transport_sock_errno));
      }
      else if (event->error_handle->error_type ==
               MQTT_ERROR_TYPE_CONNECTION_REFUSED)
      {
        ESP_LOGI(TAG, "Connection refused error: 0x%x",
                 event->error_handle->connect_return_code);
      }
      else
      {
        ESP_LOGW(TAG, "Unknown error type: 0x%x",
                 event->error_handle->error_type);
      }
      break;

    default:
      ESP_LOGI(TAG, "Other event id:%d", event->event_id);
      break;
  }
}

esp_err_t mqtt5_api_publish(const char *topic, const char *data, int len)
{
  // esp_mqtt5_client_set_publish_property(client, &publish_property);
  // ESP_LOGI(TAG, "Publish properties configured.");
  int msg_id = esp_mqtt_client_publish(client, topic, data, len, DEFAULT_QOS,
                                       DEFAULT_RETAIN);
  if (msg_id == -1)
  {
    ESP_LOGE(TAG, "Failed to publish message");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Message published, msg_id=%d", msg_id);
  return ESP_OK;
}

esp_err_t mqtt5_api_subscribe(mqtt5_api_subscription_t *subscription)
{
  int msg_id =
    esp_mqtt_client_subscribe(client, subscription->topic, DEFAULT_QOS);
  if (msg_id == -1)
  {
    ESP_LOGE(TAG, "Failed to subscribe to topic %s", subscription->topic);
    return ESP_FAIL;
  }
  _add_mqtt5_subscription(subscription);
  ESP_LOGI(TAG, "Subscribed to topic %s, msg_id=%d", subscription->topic,
           msg_id);
  return ESP_OK;
}

void mqtt5_api_start(char *broker_url, char *username, char *password,
                     uint16_t port)
{
  char uri[64];
  sprintf(uri, "%s://%s", (port == 1883 ? "mqtt" : "mqtts"), broker_url);
  esp_mqtt_client_config_t mqtt5_cfg = {
    .broker.address.uri = uri,
    .broker.address.port = port,
    // .credentials.username = username,
    // .credentials.authentication.password = password,
    .session.protocol_ver = MQTT_PROTOCOL_V_5,
    .network.disable_auto_reconnect = true,
    .session.last_will.qos = DEFAULT_QOS,
    .session.last_will.topic = "c115/last_will",
    .session.last_will.msg = "i will leave",
  };

  client = esp_mqtt_client_init(&mqtt5_cfg);

  ESP_ERROR_CHECK(esp_mqtt_client_register_event(
    client, ESP_EVENT_ANY_ID, mqtt5_api_event_handler, NULL));
  ESP_ERROR_CHECK(esp_mqtt_client_start(client));

  ESP_LOGI(TAG, "MQTT client started.");
}
