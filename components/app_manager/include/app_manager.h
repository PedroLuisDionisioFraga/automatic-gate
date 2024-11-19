/**
 * @file app_manager.h
 * @author Pedro Luis Dionísio Fraga (pedrodfraga@hotmail.com)
 *
 * @brief Application Manager
 *
 * @version 0.1
 * @date 2024-11-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include "mqtt5_api.h"
#include "wifi_api.h"

/**
 * @brief Initialize the application manager.
 *
 * This function creates the necessary tasks and event groups for managing
 * the application.
 */
void application_manager_init();

#endif // APPLICATION_MANAGER_H
