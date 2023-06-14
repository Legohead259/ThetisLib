#include "Thetis_Settings.h"

thetis_device_settings_t thetisSettings;

settingTableEntry newEntries[numNewEntries] = {
    {"ftpEnabled", FTP_ENABLED, &thetisSettings.ftpEnabled, BOOL},
    {"ftpUsername", FTP_USERNAME, &thetisSettings.ftpUsername, CHAR_ARRAY, 64},
    {"ftpPassword", FTP_PASSWORD, &thetisSettings.ftpPassword, CHAR_ARRAY, 64},
    {"accelerometerRange", ACCELEROMETER_RANGE, &thetisSettings.accelerometerRange, CHAR},
    {"gyroscopeRange", GYROSCOPE_RANGE, &thetisSettings.gyroscopeRange, CHAR},
    {"magnetometerRange", MAGNETOMETER_RANGE, &thetisSettings.magnetometerRange, CHAR},
    {"logPrintLevel", LOG_PRINT_LEVEL, &thetisSettings.logPrintLevel, CHAR},
    {"logFileLevel", LOG_FILE_LEVEL, &thetisSettings.logFileLevel, CHAR}
};

void thetisSettingsInitialize() {
    // Assign the new entries to the array
    for (int i=0; i<numNewEntries; i++) {
        settingTable[NUM_BASE_SETTINGS+1+i] = newEntries[i];
    }
}