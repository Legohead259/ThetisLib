#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>

// Configuration Data
uint8_t deviceID;
const char FW_VERSION[] = "0.4.0";
const char HW_REVISION[] = "Rev F4";
char ssid[32];
char password[32];

AsyncWebServer server(80); // Create AsyncWebServer object on port 80
#endif // THETIS_WIFIH