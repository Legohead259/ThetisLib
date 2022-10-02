#include "logging.h"

bool initTelemetryLogFile(fs::FS &fs, char * path, char * header) {
    #ifdef SDCARD_DEBUG
    Serial.print("Initializing telemetry log file...");
    #endif
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(path, "/log_%03d.csv", x);
        if (!fs.exists(path)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(path, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
        Serial.println("Unable to open file for writing");
        #endif
        return false; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
    Serial.printf("Logging to: %s\n\r", path);
    #endif

    // Write header for the log file
    writeFile(fs, path, header);
    return true;
}