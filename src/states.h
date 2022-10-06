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
} status_t;

static void updateSystemState() {
    bool _isIMUCalibrated = data.sysCal == 3 && data.accelCal == 3 && data.gyroCal == 3 && data.magCal == 3;

    if (!_isIMUCalibrated && !data.GPSFix)                      data.state = STANDBY;
    else if (_isIMUCalibrated && !data.GPSFix && !isLogging)    data.state = READY_NO_GPS;
    else if (_isIMUCalibrated && data.GPSFix  && !isLogging)    data.state = READY_GPS;
    else if (_isIMUCalibrated && !data.GPSFix && isLogging)     data.state = LOGGING_NO_GPS;
    else if (_isIMUCalibrated && data.GPSFix  && isLogging)     data.state = LOGGING_GPS;
}

static void updateSystemLED() {
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