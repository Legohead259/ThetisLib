#include "logging.h"

unsigned long logButtonStartTime = 0;
unsigned long logButtonPresses = 0;
char telemetryLogFilename[32];
char diagnosticLogFilename[32];
tmElements_t timeElements;


// ==========================
// === LOG FILE FUNCTIONS ===
// ==========================


bool initDataLogFile(fs::FS &fs) {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing telemetry log file...");
    #endif
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(telemetryLogFilename, "/log_%03d.bin", x);
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
    // char _headerBuf[512];
    // formatHeader(_headerBuf);
    // writeFile(fs, telemetryLogFilename, _headerBuf);
    return true;
}

bool logData(fs::FS &fs) {
    updateTimestamp();
    File _dataFile = fs.open(telemetryLogFilename, FILE_APPEND);
    if (!_dataFile) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.printf("Could not write to %s", telemetryLogFilename);
        #endif
        return false;
    }
    _dataFile.write((uint8_t*) &data, sizeof(data));
    _dataFile.close();
    return true;
}


// ============================
// === TIMESTAMP FUNCTIONS ===
// ============================


void updateTimestamp() {
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
    data.epoch = now();
    data.mSecond = curMSecond;
}


void getISO8601Time_GPS(char *buf) {
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", data.GPSYear, data.GPSMonth, data.GPSDay, data.GPSHour, data.GPSMinute, data.GPSSecond, data.GPSHundredth);
}

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

void syncInternalClockGPS() {
    Serial.println();
    Serial.print("Attempting to sync internal clock to GPS time...");

    if (data.GPSFix) { // If the GPS has a good fix, reset the internal clock to the GPS time
        timeElements.Year = data.GPSYear-1970;
        timeElements.Month = data.GPSMonth;
        timeElements.Day = data.GPSDay;
        timeElements.Hour = data.GPSHour;
        timeElements.Minute = data.GPSMinute;
        timeElements.Second = data.GPSSecond;

        setTime(makeTime(timeElements)); // Reset internal clock
        Serial.println("Done!");
    }
    else {
        Serial.println("GPS fix was not valid - did not sync");
    }
    Serial.println();
}


// ==================================
// === INTERRUPT SERVICE ROUTINES ===
// ==================================


void IRAM_ATTR logButtonISR() {
    logButtonPresses++;
    logButtonStartTime = millis();
}