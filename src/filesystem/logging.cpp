#include "logging.h"

unsigned long logButtonStartTime = 0;
unsigned long logButtonPresses = 0;
char telemetryLogFilename[32];
char diagnosticLogFilename[32];
tmElements_t timeElements;


// ==========================
// === LOG FILE FUNCTIONS ===
// ==========================


void formatHeader(char *buf) {
    #ifdef LOG_ALL
        #define LOG_VOLTAGE
        #define LOG_GPS
        #define LOG_IMU
        #define LOG_ORIENTATION
        #define LOG_STATE
    #endif // LOG_ALL

    #ifdef LOG_IMU
        #define LOG_ACCELEROMETER
        #define LOG_GYROSCOPE
        #define LOG_MAGNETOMETER
    #endif // LOG_IMU

    #ifdef LOG_ORIENTATION
        #define LOG_ORIENTATION_EULER
        #define LOG_ORIENTATION_QUATERNION
    #endif // LOG_ORIENTATION

    // Add timestamp header
    strcat(buf, "Timestamp ISO8601,");

    #ifdef LOG_VOLTAGE
        strcat(buf, "Voltage (V),");
    #endif // LOG_VOLTAGE

    #ifdef LOG_GPS
        strcat(buf, "GPS Fix,");
        strcat(buf, "Num Sats,");
        strcat(buf, "HDOP,");
        strcat(buf, "Latitude (deg),");
        strcat(buf, "Longitude (deg),");
        strcat(buf, "Speed (kts),");
        strcat(buf, "Course (deg),");
    #endif // LOG_GPS

    #ifdef LOG_ACCELEROMETER
        strcat(buf, "Accel Cal,");
        strcat(buf, "Accel X (m/s/s),");
        strcat(buf, "Accel Y (m/s/s),");
        strcat(buf, "Accel Z (m/s/s),");
        strcat(buf, "Lin Accel X (m/s/s),");
        strcat(buf, "Lin Accel Y (m/s/s),");
        strcat(buf, "Lin Accel Z (m/s/s),");
    #endif // LOG_ACCELEROMETER

    #ifdef LOG_GYROSCOPE
        strcat(buf, "Gyro Cal,");
        strcat(buf, "Gyro X (rad/s),");
        strcat(buf, "Gyro Y (rad/s),");
        strcat(buf, "Gyro Z (rad/s),");
    #endif // LOG_GYROSCOPE

    #ifdef LOG_MAGNETOMETER
        strcat(buf, "Mag Cal,");
        strcat(buf, "Mag X (mG),");
        strcat(buf, "Mag Y (mG),");
        strcat(buf, "Mag Z (mG),");
    #endif // LOG_VOLTAGE

    #ifdef LOG_ORIENTATION_EULER
        strcat(buf, "Roll (deg),");
        strcat(buf, "Pitch (deg),");
        strcat(buf, "Yaw (deg),");
    #endif // LOG_ORIENTATION_EULER

    #ifdef LOG_ORIENTATION_QUATERNION
        strcat(buf, "Quat W,");
        strcat(buf, "Quat X,");
        strcat(buf, "Quat Y,");
        strcat(buf, "Quat Z,");
    #endif // LOG_ORIENTATION_QUATERNION

    #ifdef LOG_STATE
        strcat(buf, "State,");
    #endif // LOG_STATE
}

bool initTelemetryLogFile(fs::FS &fs) {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing telemetry log file...");
    #endif
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(telemetryLogFilename, "/log_%03d.csv", x);
        if (!fs.exists(telemetryLogFilename)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(telemetryLogFilename, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.println("Unable to open file for writing");
        #endif
        return false; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Logging to: %s\n\r", telemetryLogFilename);
    #endif

    // Write header for the log file
    char _headerBuf[512];
    formatHeader(_headerBuf);
    writeFile(fs, telemetryLogFilename, _headerBuf);
    return true;
}

/**
bool writeTelemetryData() {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Writing telemetry packet to: %s", filename);
    #endif
    File _dataFile = SD.open(filename, FILE_APPEND);
    if (!_dataFile) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.printf("Could not write to %s", filename);
        #endif
        return false;
    }

    char _timestamp[32];
    getISO8601Time_RTC(_timestamp);
    _dataFile.print(_timestamp);
    _dataFile.printf("%0.3f,", data.voltage);
    _dataFile.printf("%d,", data.GPSFix);
    _dataFile.printf("%d,", data.numSats);
    _dataFile.printf("%d,", data.HDOP);
    _dataFile.printf("%0.3f,", data.latitude / 1E6);
    _dataFile.printf("%0.3f,", data.longitude / 1E6);
    _dataFile.printf("%0.3f,", data.GPSSpeed / 1E3);
    _dataFile.printf("%0.3f,", data.GPSCourse / 1E3);
    _dataFile.printf("%d,", data.sysCal);
    _dataFile.printf("%d,", data.gyroCal);
    _dataFile.printf("%d,", data.accelCal);
    _dataFile.printf("%d,", data.magCal);
    _dataFile.printf("%0.3f,", data.accelX);
    _dataFile.printf("%0.3f,", data.accelY);
    _dataFile.printf("%0.3f,", data.accelZ);
    _dataFile.printf("%0.3f,", data.magX);
    _dataFile.printf("%0.3f,", data.magY);
    _dataFile.printf("%0.3f,", data.magZ);
    _dataFile.printf("%0.3f,", data.gyroX);
    _dataFile.printf("%0.3f,", data.gyroY);
    _dataFile.printf("%0.3f,", data.gyroZ);
    _dataFile.printf("%0.3f,", data.roll);
    _dataFile.printf("%0.3f,", data.pitch);
    _dataFile.printf("%0.3f,", data.yaw);
    _dataFile.printf("%0.3f,", data.linAccelX);
    _dataFile.printf("%0.3f,", data.linAccelY);
    _dataFile.printf("%0.3f,", data.linAccelZ);
    _dataFile.printf("%0.3f,", data.quatW);
    _dataFile.printf("%0.3f,", data.quatX);
    _dataFile.printf("%0.3f,", data.quatY);
    _dataFile.printf("%0.3f,", data.quatZ);
    _dataFile.printf("%0.3f,", data.imuTemp);
    _dataFile.printf("%d,", data.state);
    _dataFile.print(data.packetSize);
    _dataFile.println();
    _dataFile.close();

    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Wrote to: %s\n\r", filename);
    #endif
    return true;
}
**/


// ============================
// === FORMATTING FUNCTIONS ===
// ============================


void getISO8601Time_RTC(char *buf) {
    breakTime(now(), timeElements);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (timeElements.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = timeElements.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }

    // Format timestamp into ISO8601 format
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", timeElements.Year+1970, timeElements.Month, timeElements.Day, timeElements.Hour, timeElements.Minute, timeElements.Second, curMSecond);
}


// ==================================
// === INTERRUPT SERVICE ROUTINES ===
// ==================================


void IRAM_ATTR logButtonISR() {
    logButtonPresses++;
    logButtonStartTime = millis();
}