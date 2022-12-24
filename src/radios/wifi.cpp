#include "wifi.h"

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

bool initWIFIAP() {
    diagLogger->info("Starting WiFi access point...");
    if (!WiFi.softAP(configData.ssid, "")) { // Start the access point with the config SSID and password
        diagLogger->error("Failed to start access point!");
        return false;
    }
    diagLogger->info("done!");

    IPAddress IP = WiFi.softAPIP();
    char _buf[32];
    sprintf(_buf, "AP IP address: %s", IP.toString());
    diagLogger->info(_buf);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
        diagLogger->verbose("Client requesting index!");
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
        diagLogger->verbose("Client requesting styling!");
    });

    // Start server
    server.begin();
    diagLogger->info("done!");
    return true;
}

bool initWIFIClient() {
    diagLogger->info("Starting WiFi Client...");
    // TODO: Check if SSID and password fields are available, if not return error
    if (!WiFi.begin("adafruit", "ffffffff")) {
        diagLogger->error("Failed to start WiFi client!");
        return false;
    }
    else {
        connectToWIFI(); // Attempt to connect to WiFi network
        return true;
    }
}

bool connectToWIFI() {
    char _buf[64];
    sprintf(_buf, "Connecting to SSID: %s", configData.ssid);
    diagLogger->info(_buf);
    sprintf(_buf, "Using password: %s", configData.password);
    diagLogger->info(_buf);
    unsigned long _startMillis = millis();
    while (WiFi.status() != WL_CONNECTED && millis()-_startMillis < WIFI_CONNECT_TIMEOUT*1000) { // Wait for TIMEOUT seconds while connection is tested
        delay(125);
    }

    if (WiFi.status() != WL_CONNECTED) {
        diagLogger->warn("Could not connect to WiFi network!");
        return false;
    }
    else {
        diagLogger->info("Connected!");
        sprintf(_buf, "Client IP Address: %s", WiFi.localIP());
        diagLogger->verbose(_buf);
        return true;
    }
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
        DEBUG_SERIAL_PORT.println(STR(__FIRMWARE_VERSION__));
        return STR(__FIRMWARE_VERSION__);
    }
    else if (var == "HW_REVISION") {
        DEBUG_SERIAL_PORT.println(STR(__HARDWARE_VERSION__));
        return STR(__HARDWARE_VERSION__);
    }
    DEBUG_SERIAL_PORT.println("Unknown handle");
    return var;
}