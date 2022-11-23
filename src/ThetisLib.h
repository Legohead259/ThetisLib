#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "1.2.0"

#include <Arduino.h>

// Shared data functions
#include "data.h"

// Miscellaneous functions
#include "misc/neopixel.h"
#include "misc/rtc.h"

// Filesystem functions
#include "filesystem/ThetisFS.h"
#include "filesystem/config.h"
#include "filesystem/logger.h"

// Status functions
#include "states.h"

// Sensor functions
#include "sensors/fusion.h"

// Radio functions
#include "radios/gps.h"

#ifdef WIFI_ENABLE
#include "radios/wifi.h"
#endif // WIFI_ENABLE

#endif // THETISLIB_H