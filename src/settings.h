#ifndef THETIS_SETTINGS_H
#define THETIS_SETTINGS_H

#include <Arduino.h>
#include <xioAPI/src/xioAPI.h>
#include <misc/rtc.h>

enum ThetisAPIKeyHash {
    FTP_ENABLED                     = 0xBE30BD5A,
    FTP_USERNAME                    = 0xD3C01A4F,
    FTP_PASSWORD                    = 0xED9E2942,
    LOG_PRINT_LEVEL                 = 0xFC52EEAC,
    LOG_FILE_LEVEL                  = 0x9E8BB05F,
    GPS_RTC_SYNC_ENABLED            = 0x751400E0,
    ACCELEROMETER_RANGE             = 0x9C47AA6D,
    GYROSCOPE_RANGE                 = 0xF33B8E4D,
    IMU_DATA_RATE                   = 0xDEADBEEF,
    MAGNETOMETER_PERFORMANCE_MODE   = 0xB1B5EBE4,
    MAGNETOMETER_OPERATION_MODE     = 0x362FED63,
    MAGNETOMETER_DATA_RATE          = 0xFAF2F813,
    MAGNETOMETER_RANGE              = 0x4187591A,
};

typedef struct thetis_device_settings_t {
    device_settings_t baseSettings;
    bool ftpEnabled;
    char ftpUsername[64];
    char ftpPassword[64];
    uint8_t logPrintLevel;
    uint8_t logFileLevel;
    bool gpsRtcSyncEnabled;
    uint8_t accelerometerRange;
    uint8_t gyroscopeRange;
    uint8_t imuDataRate;
    uint8_t magnetometerPerformanceMode;
    uint8_t magnetometerOperationMode;
    uint8_t magnetometerDataRate;
    uint8_t magnetometerRange;

} thetis_device_settings_t;

extern thetis_device_settings_t thetisSettings;
const size_t numNewEntries = 13;

extern settingTableEntry newEntries[numNewEntries];

void thetisSettingsInitialize();


// =========================
// === COMMAND CALLBACKS ===
// =========================


#endif // THETIS_SETTINGS_H