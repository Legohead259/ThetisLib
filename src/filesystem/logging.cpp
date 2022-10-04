#include "logging.h"

unsigned long logButtonStartTime = 0;
unsigned long logButtonPresses = 0;
char TelemetryLogFilename[32];
char diagnosticLogFilename[32];
tmElements_t timeElements;


// ==========================
// === LOG FILE FUNCTIONS ===
// ==========================


bool initTelemetryLogFile(fs::FS &fs, char * path, char * header) {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing telemetry log file...");
    #endif
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(path, "/log_%03d.csv", x);
        if (!fs.exists(path)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(path, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.println("Unable to open file for writing");
        #endif
        return false; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Logging to: %s\n\r", path);
    #endif

    // Write header for the log file
    writeFile(fs, path, header);
    return true;
}


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