#ifndef STATES_H
#define STATES_H

#include <Arduino.h>

typedef std::function<void()> StateChangeHandler;

/*
Status Table:
State           |  Color  |  Indication  |  Description
----------------|---------|--------------|---------------
Booting         |  PURPLE |   Blinking   | Thetis is booting, most likely waiting for a WiFi connection
Logging, No GPS |  BLUE   |    Solid     | Thetis is logging, but does not have a GPS fix
Logging, GPS    |  GREEN  |    Solid     | Thetis is logging with a GPS fix
Ready, No GPS   |  BLUE   |   Blinking   | Accelerometer is calibrated but no GPS fix
Ready, GPS      |  GREEN  |   Blinking   | Accelerometer is calibrated and there is a GPS fix
Standby         |  AMBER  |    Solid     | Accelerometer is not calibrated yet
*/

typedef enum system_state_t {
    ERROR = -1,
    BOOTING,
    LOGGING_NO_GPS,
    LOGGING_GPS,
    READY_NO_GPS,
    READY_GPS,
    STANDBY
} system_state_t;

/**
Code Table:
Error       | RED | YELLOW | Description
------------|-----|--------|-------------------------------------------------------
General     |  1  |    1   | Generalized failure state
Settings    |  1  |    2   | Board is initialized with/encounters invalid settings
Low Battery |  2  |    1   | Battery voltage is below the threshold limit
Battery IC  |  2  |    2   | The battery monitoring IC fails to initialize
Temperature |  2  |    3   | The reported system temperature exceeds a threshold
Card Mount  |  3  |    1   | Filesystem fails to mount
Card Type   |  3  |    2   | Filesystem initializes, but reports a bad card type
File Ops    |  3  |    3   | A requested file operation fails to execute
WiFi Radio  |  4  |    1   | Radio fails to initialize/encounters error
BT Radio    |  4  |    2   | Radio fails to initialize/encounters error
GPS         |  4  |    3   | GPS radio encounters an error
IMU         |  5  |    1   | IMU encounters and error
MAG         |  5  |    2   | Magnetometer encounters and error
Fusion      |  5  |    3   | Sensor fusion algorithm encounters an error
CANbus      |  6  |    1   | The CAN interface encounters an error
Device ID   |  6  |    2   | The device ID is not properly configured 
*/

typedef enum error_state_t {
    NO_ERROR,
    GENERAL_ERROR,
    SETTINGS_ERROR,
    LOW_BATTERY_ERROR,
    BATTERY_MONITOR_ERROR,
    TEMPERATURE_ERROR,
    FILESYSTEM_MOUNT_ERROR,
    CARD_TYPE_ERROR,
    FILE_OPERATION_ERROR,
    WIFI_RADIO_ERROR,
    BLUETOOTH_RADIO_ERROR,
    GPS_ERROR,
    IMU_ERROR,
    MAGNETOMETER_ERROR,
    FUSION_ERROR,
    CANBUS_ERROR,
    DEVICE_ID_ERROR
} error_state_t;

class StateBaseClass {
public:
    virtual const char* c_str() { return ""; }

    int8_t getState() {return state; }
    void setState(int8_t s);
    void setStateChangeCallback(StateChangeHandler cbPtr) { stateChangeCallback = cbPtr; }

protected:
    int8_t state;
    int8_t lastState;
    StateChangeHandler stateChangeCallback = nullptr;
};

class SystemStatus : public StateBaseClass {
public:
    const char* c_str();

private:
    int8_t state = BOOTING;
    int8_t lastState = -2;
};

class SystemError : public StateBaseClass {
public:
    const char* c_str();
private:
    int8_t state = NO_ERROR;
    int8_t lastState = -1;
};

#endif // STATES_H