#ifndef THETIS_GPS_H
#define THETIS_GPS_H

#include <utility/MicroNMEA.h>

#define GPS_SYNC_INTERVAL 1 // minutes
#define GPS_TIMEOUT 5000 // ms

// TODO: Implement MTK3339 API
// TODO: Make GPS object-oriented

typedef enum {
    BAUD_9600 = 9600,
    BAUD_38400 = 38400
} gps_baudrates_t;

typedef enum {
    REFRESH_RATE_CMD = 220,
    BAUDRATE_CMD = 251,
    SENTENCE_TYPES_CMD = 14
} gps_commands_t;

extern HardwareSerial& GPS;
extern char nmeaBuffer[100];
extern MicroNMEA nmea;
extern bool ledState;
extern volatile bool ppsTriggered;

bool initGPS();

void getISO8601Time(char *buf);
void syncInternalClockGPS();

#endif // ndef THETIS_GPS_H