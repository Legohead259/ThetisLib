#ifndef THETISLIB_H
#define THETISLIB_H
#define __THETISLIB_VERSION__ "1.6.0"

#include <Arduino.h>

// Shared data functions
#include "data.h"

// Miscellaneous functions
#include "misc/neopixel.h"
#include "misc/rtc.h"

// Filesystem functions
#include "filesystem/filesystem.h"
#include "filesystem/logger.h"

// Status functions
#include "states.h"

// Sensor functions
#include "sensors/lsm6dso32.h"
#ifdef MAG_ENABLE
#include "sensors/LIS3MDL.h"
#endif // MAG_ENABLE
#include "fusion/Fusion/fusion.h"

// Radio functions
#include "radios/gps.h"

// WiFi functions
#ifdef WIFI_ENABLE
#include "radios/wifi.h"
#endif

// Revision-specific functions
#ifdef BATT_MON_ENABLE
#include "sensors/max1704.h"
#endif // BATT_MON_ENABLE

#include "timing/TimerEvents.h"

#include "xioAPI/src/xioAPI.h"

#include "settings.h"



#endif // THETISLIB_H