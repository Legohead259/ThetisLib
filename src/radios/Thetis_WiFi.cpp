#include "Thetis_WiFi.h"

// Configuration Data
uint8_t deviceID;
const char FW_VERSION[] = "0.4.0";
const char HW_REVISION[] = "Rev F4";
char ssid[32];
char password[32];

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

bool initializeThetisAPI() {
    // TODO: Actually implement
    
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    return true;
}

String processor(const String &var) {
    Serial.print(var); Serial.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", deviceID);
        Serial.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        Serial.println(FW_VERSION);
        return FW_VERSION;
    }
    else if (var == "HW_REVISION") {
        Serial.println(HW_REVISION);
        return HW_REVISION;
    }
    Serial.println();
    return var;
}