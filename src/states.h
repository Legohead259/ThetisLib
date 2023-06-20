#ifndef STATES_H
#define STATES_H

#include "filesystem/logger.h"

/*
Status Table:
State           |  Color  |  Indication  |  Description
----------------|---------|--------------|---------------
Booting         |  PURPLE |   Pulsing    | Thetis is booting, most likely waiting for a WiFi connection
Logging, No GPS |  BLUE   |    Solid     | Thetis is logging, but does not have a GPS fix
Logging, GPS    |  GREEN  |    Solid     | Thetis is logging with a GPS fix
Ready, No GPS   |  BLUE   |   Pulsing    | Accelerometer is calibrated but no GPS fix
Ready, GPS      |  GREEN  |   Pulsing    | Accelerometer is calibrated and there is a GPS fix
Standby         |  AMBER  |    Solid     | Accelerometer is not calibrated yet
*/

typedef enum Status {
    ERROR = -1,
    BOOTING,
    LOGGING_NO_GPS,
    LOGGING_GPS,
    READY_NO_GPS,
    READY_GPS,
    STANDBY
} status_t;

static int8_t _lastState = -2;

inline void getStateString(char* buf, int8_t s) {
    switch (s) {
        case ERROR:
            strcpy(buf, "ERROR");
            break;
        case BOOTING:
            strcpy(buf, "BOOTING");
            break;
        case LOGGING_NO_GPS:
            strcpy(buf, "LOGGING, NO GPS");
            break;
        case LOGGING_GPS:
            strcpy(buf, "LOGGING WITH GPS");
            break;
        case READY_NO_GPS:
            strcpy(buf, "READY, NO GPS");
            break;
        case READY_GPS:
            strcpy(buf, "READY WITH GPS");
            break;
        case STANDBY:
            strcpy(buf, "STANDBY");
            break;
        default:
            strcpy(buf, "");
            break;
    }
}

inline void setSystemState(int8_t s) {
    if (data.state == s) return; // Do not change system state if there is no difference
    
    char stateStr[20];
    getStateString(stateStr, s);
    diagLogger->info("Setting system state to: %s", stateStr);
    _lastState = data.state;
    data.state = s;
}

inline void getSystemState(char* buf) {
    getStateString(buf, data.state);
}

inline void updateSystemState() {
    // TODO: Determine better method for IMU calibration
    // bool _isIMUCalibrated = data.sysCal == 3 && data.accelCal == 3 && data.gyroCal == 3 && data.magCal == 3;
    bool _isIMUCalibrated = true; // Override since we are not calculating calibration, yet

    if (!_isIMUCalibrated && !data.GPSFix)                      setSystemState(STANDBY);
    else if (_isIMUCalibrated && !data.GPSFix && !isLogging)    setSystemState(READY_NO_GPS);
    else if (_isIMUCalibrated && data.GPSFix  && !isLogging)    setSystemState(READY_GPS);
    else if (_isIMUCalibrated && !data.GPSFix && isLogging)     setSystemState(LOGGING_NO_GPS);
    else if (_isIMUCalibrated && data.GPSFix  && isLogging)     setSystemState(LOGGING_GPS);
}

inline void updateSystemLED() {
    switch (data.state) {
        case LOGGING_NO_GPS:
            pixel.setPixelColor(0, BLUE); pixel.show(); // Glow solid blue
            break;
        case LOGGING_GPS:
            pixel.setPixelColor(0, GREEN); pixel.show(); // Glow solid green
            break;
        case READY_NO_GPS:
            pulseLED(BLUE); // Pulse blue
            break;
        case READY_GPS:
            pulseLED(GREEN); // Pulse green
            break;
        case STANDBY:
            pixel.setPixelColor(0, 255, 191, 0); pixel.show(); // Glow solid amber
            break;
        case BOOTING:
            pulseLED(PURPLE); // Pulse purple
            break;
        default:
            pixel.setPixelColor(0, RED); pixel.show(); // Turn off LED
            break;
    }
}

#endif // STATES_H