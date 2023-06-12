#ifndef THETIS_SETTINGS_H
#define THETIS_SETTINGS_H

#include <Arduino.h>
#include <xioAPI/src/xioAPI_Settings.h>

enum ThetisAPIKeyHash {
    FTP_ENABLED = 0xBE30BD5A
};

typedef struct thetis_device_settings_t {
    device_settings_t baseSettings;
    bool ftpEnabled;
} thetis_device_settings_t;

static thetis_device_settings_t thetisSettings;
const size_t numNewEntries = 1;

settingTableEntry newEntries[numNewEntries] = {
    {FTP_ENABLED, &thetisSettings.ftpEnabled, BOOL}
};

size_t currentSize = sizeof(settingTable) / sizeof(settingTableEntry);
size_t newSize = currentSize + numNewEntries;

void thetisSettingsInitialize() {
    thetisSettings.ftpEnabled = true;

    // Assign the new entries to the array
    for (int i=0; i<numNewEntries; i++) {
        settingTable[NUM_SETTINGS+1+i] = newEntries[i];
    }
}

#endif // THETIS_SETTINGS_H