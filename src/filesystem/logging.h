#ifndef LOGGING_H
#define LOGGING_H

#include <FS.h>

// Function prototype definitions
bool writeFile(fs::FS &fs, const char * path, const char * message);

extern char TelemetryLogFilename[32];
extern char diagnosticLogFilename[32];

bool initTelemetryLogFile(fs::FS &fs, char * path, char * header);

#endif // LOGGING_H