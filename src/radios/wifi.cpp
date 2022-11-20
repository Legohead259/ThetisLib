#include "wifi.h"

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

bool initWIFIAP() {
    #ifdef WIFI_DEBUG
    DEBUG_SERIAL_PORT.print("Starting WiFi access point...");
    #endif
    sprintf(configData.ssid, "Thetis-%03u", configData.DEVICE_ID); // Format AP SSID based on Device ID
    if (!WiFi.softAP(configData.ssid, "")) { // Start the access point with the DEVICE_ID SSID and no password
        #ifdef WIFI_DEBUG
        DEBUG_SERIAL_PORT.println("Failed to start access point!");
        #endif
        return false;
    }
    #ifdef WIFI_DEBUG
    DEBUG_SERIAL_PORT.println("done!");
    #endif

    IPAddress IP = WiFi.softAPIP();
    #ifdef WIFI_DEBUG
    DEBUG_SERIAL_PORT.print("AP IP address: ");
    DEBUG_SERIAL_PORT.println(IP);
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
    DEBUG_SERIAL_PORT.print(var); DEBUG_SERIAL_PORT.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", configData.DEVICE_ID);
        DEBUG_SERIAL_PORT.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        DEBUG_SERIAL_PORT.println(configData.FW_VERSION);
        return configData.FW_VERSION;
    }
    else if (var == "HW_REVISION") {
        DEBUG_SERIAL_PORT.println(configData.HW_REVISION);
        return configData.HW_REVISION;
    }
    DEBUG_SERIAL_PORT.println("Unknown handle");
    return var;
}