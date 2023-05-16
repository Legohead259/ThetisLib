#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "1.5.2"

#include <Arduino.h>

// Shared data functions
#include "data.h"

// Miscellaneous functions
#include "misc/neopixel.h"
#include "misc/rtc.h"
#include "misc/timer.h"

// Filesystem functions
#include "filesystem/files.h"
#include "filesystem/config.h"
#include "filesystem/logger.h"

// Status functions
#include "states.h"

// Sensor functions
#include "sensors/lsm6dso32.h"
#ifdef MAG_ENABLE
#include "sensors/LIS3MDL.h"
#endif // MAG_ENABLE
#include "fusion/fusion.h"
// #include "MadgwickAHRS/src/MadgwickAHRS.h"
// #include "AHRS/fusion.h"

// Radio functions
#include "radios/gps.h"

// WiFi functions
#ifdef WIFI_ENABLE
#include "radios/wifi.h"
#endif

// Revision-specific functions
#if defined(REV_F5) || defined(REV_G2)
#include "sensors/max1704.h"
#endif // defined(REV_F5) || defined(REV_G2)

#endif // THETISLIB_H