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