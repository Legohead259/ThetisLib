#ifndef THETIS_GPS_H
#define THETIS_GPS_H

#include <MicroNMEA.h>
#include "../data.h"
#include "../filesystem/logger.h"

#define GPS_POLL_INTERVAL 100 // ms
#define GPS_SYNC_INTERVAL 1 // minutes
#define GPS_TIMEOUT 5000 // ms

// TODO: Implement Kalman filtering for GPS data

#define r_lat 	1.5 // GPS latitude measurement noise (m - radius of error)
#define r_lon 	1.5 // GPS longitude measurement noise (m - radius of error)
#define r_sog 	1.0 // GPS SOG measurement noise (m/s)
#define r_cog 	1.0 // GPS COG measurement noise (deg)
#define q_gps 	0.5 // Process error - gps

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
void pollGPS();

void printUnknownSentence(MicroNMEA &nmea);


#endif // ndef THETIS_GPS_H