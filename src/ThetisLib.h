#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "1.2.0"

#include <Arduino.h>

// Status functions
#include "states.h"

// Telemetry data functions
#include "telemetry.h"

// Filesystem functions
#include "filesystem/ThetisFS.h"
#include "filesystem/config.h"
#include "filesystem/logging.h"

// Timestamp functions
#include <TimeLib.h>
extern tmElements_t timeElements;
void getISO8601Time_RTC(char *buf);

// Sensor functions
#include "sensors/bno055.h"
#include "sensors/lsm6dso32.h" 

// Radio functions
#include "radios/gps.h"
#include "radios/wifi.h"

// Miscellaneous functions
#include "misc/neopixel.h"

#endif // THETISLIB_H