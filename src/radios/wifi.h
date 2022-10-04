#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <AsyncElegantOTA.h>
#include "../filesystem/config.h"

extern AsyncWebServer server; // Create AsyncWebServer object on port 80

// Prototypes
String processor(const String &var);
#endif // THETIS_WIFIH