#include <ThetisLib.h>


// =========================
// === GENERAL FUNCTIONS ===
// =========================


bool writeTelemetryData(fs::FS &fs, const char * path, telemetry_t &data, Stream &out) {
    out.printf("Writing telemetry packet to: %s", path);
    File _dataFile = fs.open(path, FILE_APPEND);
    if (!_dataFile) {
        out.printf("Could not write to %s", path);
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

    out.printf("Wrote to: %s\n\r", path);
    return true;
}

tmElements_t tm;

void getISO8601Time_RTC(char *buf) {
    breakTime(now(), tm);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (tm.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = tm.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }

    // Format timestamp into ISO8601 format
    // sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, curMSecond);
}

// =========================
// === LOGGING FUNCTIONS ===
// =========================


long logButtonStartTime = 0;
// bool logButtonPressed = false;
long logButtonPresses = 0;

void IRAM_ATTR logButtonISR() {
    // if (digitalRead(LOG_EN)) {             // Button is Pressed
    //     logButtonPressed = true;            // Set the log button pressed flag
    //     logButtonStartTime = millis();      // Start the log button hold timer
    //     // Serial.println("Button pressed!");    // DEBUG
    //     // Serial.printf("logButtonPressed: %s\n", logButtonPressed ? "true" : "false"); // DEBUG
    //     // Serial.printf("logButtonStartTime: %d\n\n", logButtonStartTime); // DEBUG
    // }
    // else {
    //     logButtonPressed = false;           // reset the log button pressed flag
    //     // Serial.println("Button released!");   // DEBUG
    //     // Serial.printf("logButtonPressed: %s\n\n", logButtonPressed ? "true" : "false"); // DEBUG
    // }
    logButtonPresses++;
    logButtonStartTime = millis();
}