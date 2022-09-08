#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include "../filesystem/ThetisConfig.h"
#include <AsyncElegantOTA.h>

// Configuration Data
// extern uint8_t deviceID;
// extern const char FW_VERSION[];
// extern const char HW_REVISION[];
// extern char ssid[32];
// extern char password[32];

extern AsyncWebServer server; // Create AsyncWebServer object on port 80

// Prototypes
String processor(const String &var);
#endif // THETIS_WIFIH