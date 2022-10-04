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
    BOOTING,
    LOGGING_NO_GPS,
    LOGGING_GPS,
    READY_NO_GPS,
    READY_GPS,
    STANDBY
} Status_t;