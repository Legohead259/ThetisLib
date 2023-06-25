#include "settings.h"

thetis_device_settings_t thetisSettings;

settingTableEntry newEntries[numNewEntries] = {
    {"ftpEnabled", FTP_ENABLED, &thetisSettings.ftpEnabled, BOOL},
    {"ftpUsername", FTP_USERNAME, &thetisSettings.ftpUsername, CHAR_ARRAY, 64},
    {"ftpPassword", FTP_PASSWORD, &thetisSettings.ftpPassword, CHAR_ARRAY, 64},
    {"logPrintLevel", LOG_PRINT_LEVEL, &thetisSettings.logPrintLevel, CHAR},
    {"logFileLevel", LOG_FILE_LEVEL, &thetisSettings.logFileLevel, CHAR},
    {"gpsRtcSyncEnabled", GPS_RTC_SYNC_ENABLED, &thetisSettings.gpsRtcSyncEnabled, BOOL},
    {"accelerometerRange", ACCELEROMETER_RANGE, &thetisSettings.accelerometerRange, CHAR},
    {"gyroscopeRange", GYROSCOPE_RANGE, &thetisSettings.gyroscopeRange, CHAR},
    {"imuDataRate", IMU_DATA_RATE, &thetisSettings.imuDataRate, CHAR},
    {"magnetometerPerformanceMode", MAGNETOMETER_PERFORMANCE_MODE, &thetisSettings.magnetometerPerformanceMode, CHAR},
    {"magnetometerOperationMode", MAGNETOMETER_OPERATION_MODE, &thetisSettings.magnetometerOperationMode, CHAR},
    {"magnetometerDataRate", MAGNETOMETER_DATA_RATE, &thetisSettings.magnetometerDataRate, CHAR},
    {"magnetometerRange", MAGNETOMETER_RANGE, &thetisSettings.magnetometerRange, CHAR},
    {"gaugeResetVoltage", GAUGE_RESET_VOLTAGE, &thetisSettings.gaugeResetVoltage, FLOAT},
    {"gaugeActivityThreshold", GAUGE_ACTIVITY_THRESHOLD, &thetisSettings.gaugeActivityThreshold, FLOAT},
    {"gaugeHibernationThreshold", GAUGE_HIBERNATION_THRESHOLD, &thetisSettings.gaugeHibernationThreshold, FLOAT},
    {"gaugeAlertMinVoltage", GAUGE_ALERT_MIN_VOLTAGE, &thetisSettings.gaugeAlertMinVoltage, FLOAT},
    {"gaugeAlertMaxVoltage", GAUGE_ALERT_MAX_VOLTAGE, &thetisSettings.gaugeAlertMaxVoltage, FLOAT}
};

void thetisSettingsInitialize() {
    // Assign the new entries to the array
    for (int i=0; i<numNewEntries; i++) {
        settingTable[NUM_BASE_SETTINGS+1+i] = newEntries[i];
    }

    api.setCmdReadTimeCallback([]() {
        char _buf[64];
        getTime_RTC(_buf);
        api.send(true, "{time:%s}", _buf);
    });

    api.setCmdWriteTimeCallback([]() {
        syncInternalClock(api.getValue<const char*>());
        api.cmdReadTime();
    });

    api.setCmdResetCallback([]() { 
        api.send(true, "{\"reset\":null}");
        ESP.restart(); 
    });

    api.setCmdShutdownCallback([]() {
        api.send(true, "{\"shutdown\":null}");
        ESP.restart();
        // TODO: Add an auto-latching shutoff circuit to hardware
    });

    api.setCmdStrobeCallback([]() {
        // TODO: Implement asynchronous strobing
    });

    api.setCmdColourCallback([]() {
        // TODO: Call neopixel color change function
    });

    api.setCmdHeadingCallback([]() {
        // TODO: Reset AHRS heading to specified value IF `magnetometerIgnoreEnabled` is TRUE
    });

    api.setCmdSerialAccessoryCallback([]() { 
        // Ignore this command for Thetis
        return; 
    });

    api.setCmdFormatCallback([]() {
        // TODO: Call filesystem `format()` function
        // TODOTODO: Create a filesystem `format()` function
    });

    api.setCmdSelfTestCallback([]() {
        // TODO: Call self-test function
        // TODOTODO: Create a self-test initialization function
    });

    api.setCmdBootloaderCallback([]() {
        // This is not possible on the ESP32 platform
        return;
    });

    api.setCmdFactoryCallback([]() {
        // TODO: Put the device into a factory mode state for writing to READ-ONLY settings
        // TODOTODO: Make it so only some values can be written to whilst in factory mode
    });

    api.setCmdEraseCallback([]() {
        // TODO: Call filesystem (SPIFFS) `erase()` function
        // TODOTODO: Create filesystem (SPIFFS) `erase()` function
    });
}