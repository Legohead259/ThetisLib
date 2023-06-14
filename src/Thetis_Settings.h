#ifndef THETIS_SETTINGS_H
#define THETIS_SETTINGS_H

#include <Arduino.h>
#include <xioAPI/src/xioAPI_Settings.h>

enum ThetisAPIKeyHash {
    FTP_ENABLED = 0xBE30BD5A,
    FTP_USERNAME = 0xD3C01A4F,
    FTP_PASSWORD = 0xED9E2942,
    ACCELEROMETER_RANGE = 0x9C47AA6D,
    GYROSCOPE_RANGE = 0xF33B8E4D,
    MAGNETOMETER_RANGE = 0x4187591A,
    LOG_PRINT_LEVEL = 0xFC52EEAC,
    LOG_FILE_LEVEL = 0x9E8BB05F
};

typedef struct thetis_device_settings_t {
    device_settings_t baseSettings;
    bool ftpEnabled;
    char ftpUsername[64];
    char ftpPassword[64];
    uint8_t accelerometerRange;
    uint8_t gyroscopeRange;
    uint8_t magnetometerRange;
    uint8_t logPrintLevel;
    uint8_t logFileLevel;
} thetis_device_settings_t;

extern thetis_device_settings_t thetisSettings;
const size_t numNewEntries = 8;

extern settingTableEntry newEntries[numNewEntries];

void thetisSettingsInitialize();

#endif // THETIS_SETTINGS_H