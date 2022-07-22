#ifndef THETIS_WIFIH
#define THETIS_WIFIH
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

char ssid[32];
char password[32];
AsyncWebServer server(80); // Create AsyncWebServer object on port 80
#endif // THETIS_WIFIH