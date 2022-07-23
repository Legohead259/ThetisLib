#include "Thetis_WiFi.h"

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

String processor(const String &var, ConfigData *cfgData) {
    Serial.print(var); Serial.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", cfgData->deviceID);
        Serial.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        Serial.println(cfgData->FW_VERSION);
        return FW_VERSION;
    }
    else if (var == "HW_REVISION") {
        Serial.println(cfgData->HW_REVISION);
        return HW_REVISION;
    }
    Serial.println("Unknown handle");
    return var;
}