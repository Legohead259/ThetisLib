#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "1.2.0"

#include <Arduino.h>

// Telemetry data functions
#include "telemetry.h"

// Miscellaneous functions
#include "misc/neopixel.h"

// Filesystem functions
#include "filesystem/ThetisFS.h"
#include "filesystem/config.h"
#include "filesystem/logging.h"

// Status functions
#include "states.h"

// Sensor functions
#include "sensors/bno055.h"
#include "sensors/lsm6dso32.h" 

// Radio functions
#include "radios/gps.h"
#include "radios/wifi.h"

#endif // THETISLIB_H