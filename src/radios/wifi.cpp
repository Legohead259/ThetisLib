#include "wifi.h"

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

bool initWIFIAP() {
    #ifdef WIFI_DEBUG
    Serial.print("Starting WiFi access point...");
    #endif
    sprintf(configData.ssid, "Thetis-%03u", configData.deviceID); // Format AP SSID based on Device ID
    if (!WiFi.softAP(configData.ssid, "")) { // Start the access point with the deviceID SSID and no password
        #ifdef WIFI_DEBUG
        Serial.println("Failed to start access point!");
        #endif
        return false;
    }
    #ifdef WIFI_DEBUG
    Serial.println("done!");
    #endif

    IPAddress IP = WiFi.softAPIP();
    #ifdef WIFI_DEBUG
    Serial.print("AP IP address: ");
    Serial.println(IP);
    #endif

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    // Start server
    server.begin();
}

String processor(const String &var) {
    Serial.print(var); Serial.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", configData.deviceID);
        Serial.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        Serial.println(configData.FW_VERSION);
        return configData.FW_VERSION;
    }
    else if (var == "HW_REVISION") {
        Serial.println(configData.HW_REVISION);
        return configData.HW_REVISION;
    }
    Serial.println("Unknown handle");
    return var;
}