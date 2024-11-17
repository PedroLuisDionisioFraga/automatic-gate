# Application Manager
Manages the lifecycle of applications on the device.

## WiFi Setup
To configure the WiFi credentials create a folder named `secrets` in the root of the application and create a file named `wifi_secrets.h` with the following content:
```c
#ifndef WIFI_SECRETS_H
#define WIFI_SECRETS_H

#define WIFI_SSID "your wifi ssid"
#define WIFI_PASSWORD "your wifi password"


#endif // WIFI_SECRETS_H
```
**NOTE**: The `secrets` folder is added to the `.gitignore` file so that it is not committed to the repository.
