#ifndef LOGGING_H
#define LOGGING_H

#include <FS.h>
#include <TimeLib.h>
#include "../data.h"

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
void formatHeader(char *buf);
bool initDataLogFile(fs::FS &fs);
bool logData(fs::FS &fs);
bool logDataBin(fs::FS &fs);

// Timestamp functions
void updateTimestamp();
void getISO8601Time_GPS(char *buf);
void getISO8601Time_RTC(char *buf);
void syncInternalClockGPS();

// Logging ISRs
void IRAM_ATTR logButtonISR();

#endif // LOGGING_H