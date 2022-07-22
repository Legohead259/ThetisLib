#include "WiFi.h"
#include "ESPAsyncWebServer.h"

char ssid[32];
char password[32];
AsyncWebServer server(80); // Create AsyncWebServer object on port 80