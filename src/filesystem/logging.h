#ifndef LOGGING_H
#define LOGGING_H

#include <FS.h>
#include <TimeLib.h>

#define LOG_BTN_HOLD_TIME 500 // ms

extern unsigned long logButtonPresses;
extern unsigned long logButtonStartTime;
extern bool isLogging;

extern char TelemetryLogFilename[32];
extern char diagnosticLogFilename[32];

extern tmElements_t timeElements;

// Forward declarations
bool writeFile(fs::FS &fs, const char * path, const char * message);

// Log file functions
bool initTelemetryLogFile(fs::FS &fs, char * path, char * header);

// Logging ISRs
void IRAM_ATTR logButtonISR();

#endif // LOGGING_H