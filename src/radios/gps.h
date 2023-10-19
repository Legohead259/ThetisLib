#ifndef THETIS_GPS_H
#define THETIS_GPS_H

#include <MicroNMEA.h>
#include "../filesystem/logger.h"
#include "../subsystem.h"

#define GPS_POLL_INTERVAL 100 // ms
#define GPS_SYNC_INTERVAL 1 // minutes
#define GPS_TIMEOUT 5000 // ms

// TODO: Implement Kalman filtering for GPS data
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

class ThetisGPS : public MicroNMEA, ThetisSubsystem {
public:
    ThetisGPS() : MicroNMEA(nmeaBuffer, sizeof(nmeaBuffer)) {}

    bool begin() override;
    void poll() override;
    void updateSettings() override;
    void test() override {}
    
    void getISO8601Time_GPS(char* buf);
private:
    HardwareSerial& serialPort = GPS_SERIAL_PORT;
    bool ppsTriggered = false;
    char nmeaBuffer[100];
    
    static void printUnknownSentence(MicroNMEA &nmea) { /*diagLogger->warn("Unknown sentence: %s", nmea.getSentence());*/ }
    static void ppsHandler(bool& ppsPtr) { ppsPtr = true; }
};

extern ThetisGPS gps;

#endif // ndef THETIS_GPS_H