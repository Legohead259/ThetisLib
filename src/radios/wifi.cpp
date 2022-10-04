#include "wifi.h"


AsyncWebServer server(80); // Create AsyncWebServer object on port 80

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