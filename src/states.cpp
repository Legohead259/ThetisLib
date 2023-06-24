#include "states.h"

void StateBaseClass::setState(int8_t s) {
    if (state == s) return; // Do not change system state if there is no difference

    lastState = state;
    state = s;

    if (stateChangeCallback != nullptr) stateChangeCallback();
}

const char* SystemStatus::c_str() {
    switch (state) {
        case ERROR:
            return "ERROR";
        case BOOTING:
            return "BOOTING";
        case LOGGING_NO_GPS:
            return "LOGGING, NO GPS";
        case LOGGING_GPS:
            return "LOGGING WITH GPS";
        case READY_NO_GPS:
            return "READY, NO GPS";
        case READY_GPS:
            return "READY WITH GPS";
        case STANDBY:
            return "STANDBY";
        default:
            return "";
    }
}

const char* SystemError::c_str() {
    switch (state) {
        case GENERAL_ERROR:
            return "GENERAL ERROR";
        case SETTINGS_ERROR:
            return "SETTINGS ERROR";
        case LOW_BATTERY_ERROR:
            return "LOW BATTERY ERROR";
        case BATTERY_MONITOR_ERROR:
            return "BATTERY MONITOR ERROR";
        case TEMPERATURE_ERROR:
            return "TEMPERATURE ERROR";
        case FILESYSTEM_MOUNT_ERROR:
            return "FILESYSTEM MOUNT ERROR";
        case CARD_TYPE_ERROR:
            return "CARD TYPE ERROR";
        case FILE_OPERATION_ERROR:
            return "FILE OPERATION ERROR";
        case WIFI_RADIO_ERROR:
            return "WIFI RADIO ERROR";
        case BLUETOOTH_RADIO_ERROR:
            return "BLUETOOTH RADIO ERROR";
        case GPS_ERROR:
            return "GPS ERROR";
        case IMU_ERROR:
            return "IMU ERROR";
        case MAGNETOMETER_ERROR:
            return "MAGNETOMETER ERROR";
        case FUSION_ERROR:
            return "FUSION ERROR";
        case CANBUS_ERROR:
            return "CANBUS ERROR";
        case DEVICE_ID_ERROR:
            return "DEVICE ID ERROR";
        default:
            return "";
    }
}