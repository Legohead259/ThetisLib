#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "1.2.0"

#include <Arduino.h>

// Shared data functions
#include "data.h"

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

#ifdef WIFI_ENABLE
#include "radios/wifi.h"
#endif // WIFI_ENABLE

#endif // THETISLIB_H