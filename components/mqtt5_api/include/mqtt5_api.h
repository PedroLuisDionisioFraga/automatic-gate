/**
 * @file mqtt5_api.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief MQTT 5 API header file
 *
 * @version 0.1
 * @date 2024-11-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MQTT5_API_H
#define MQTT5_API_H

#include <esp_err.h>
#include <stdint.h>

/**
 * @brief New type is for a function pointer.
 *
 */
typedef void (*mqtt5_api_callback_t)(char *data, int len);

/**
 * @brief Structure to hold MQTT 5.0 API subscription information.
 *
 * This structure holds the topic, quality of service (QoS), and callback
 * function for an MQTT 5.0 API subscription.
 */
typedef struct
{
  const char *topic;
  int qos;
  mqtt5_api_callback_t callback;
} mqtt5_api_subscription_t;

/**
 * @brief Start the MQTT client.
 *
 * This function initializes and starts the MQTT client with the specified
 * broker URL, username, password, and port.
 *
 * @param broker_url The URL of the MQTT broker.
 * @param username The username for MQTT authentication.
 * @param password The password for MQTT authentication.
 * @param port The port for MQTT connection (1883 for MQTT, 8883 for MQTT over
 * SSL).
 */
void mqtt5_api_start(char *broker_url, char *username, char *password,
                     uint16_t port);

/**
 * @brief Publish a message to an MQTT topic.
 *
 * This function publishes a message to the specified MQTT topic with the given
 * quality of service (QoS) and retain flag.
 *
 * @param topic The MQTT topic to publish to.
 * @param data The message data to publish.
 * @param len The length of the message data.
 * @param qos The quality of service level (0, 1, or 2).
 * @param retain The retain flag (0 or 1).
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t mqtt5_api_publish(const char *topic, const char *data, int len,
                            int qos, int retain);

/**
 * @brief Subscribe to an MQTT topic.
 *
 * This function subscribes to the specified MQTT topic with the given quality
 * of service (QoS).
 *
 * @param topic The MQTT topic to subscribe to.
 * @param qos The quality of service level (0, 1, or 2).
 * @return ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t mqtt5_api_subscribe(mqtt5_api_subscription_t *subscription);

#endif // MQTT5_API_H
