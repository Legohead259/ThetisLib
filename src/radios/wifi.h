#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
// #include <AsyncElegantOTA.h>
#include "../filesystem/config.h"
#include "../filesystem/logger.h"

#define ST(A) #A
#define STR(A) ST(A)

#define WIFI_CONNECT_TIMEOUT 10 // seconds

extern AsyncWebServer server; // Create AsyncWebServer object on port 80

bool initWIFIAP();
bool initWIFIClient();
bool connectToWIFI();

// Prototypes
String processor(const String &var);
#endif // THETIS_WIFIH