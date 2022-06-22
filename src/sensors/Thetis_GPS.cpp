#include "Thetis_GPS.h"

// ============================
// === GPS MODULE FUNCTIONS ===
// ============================


HardwareSerial& GPS = GPS_SERIAL_PORT;
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
bool ledState = LOW;
volatile bool ppsTriggered = false;

bool initGPS(HardwareSerial &gps, Stream &out) {
    out.print("Initializing GPS..."); // DEBUG
    gps.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!gps) {
        out.println("Failed to initialize GPS"); // DEBUG
        return false;
    }
    delay(10);
    MicroNMEA::sendSentence(gps, "$PMTK251,38400");         // Set GPS baudrate to 38400
    delay(10);
    gps.begin(38400);
    MicroNMEA::sendSentence(gps, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    MicroNMEA::sendSentence(gps, "$PMTK220,100");           // Set GPS update rate to 100 ms (10 Hz)

    // TODO: implement a check for good GPS data
    out.println("done!"); // DEBUG
    return true;
}

// TODO: Implement MTK3339 API 

// void changeBaudrate(gps_baudrates_t baud) {
//     sendCommand(BAUDRATE_CMD, )
// }

// void sendCommand(gps_commands_t cmd, char *payload[], HardwareSerial &gps, Stream &out) {
//     char _buf[64];
//     sprintf(_buf, "$PMTK%d,%s", cmd, payload);
//     MicroNMEA::sendSentence(gps, _buf);
//     out.printf("Sending: %s\n\r", _buf);
// }

// =========================
// === WRAPPER FUNCTIONS ===
// =========================


void getISO8601Time(char *buf, long curMillis) {
    static long _oldMillis = 0;
    static uint8_t _oldSecond = 0;
    if (nmea.getSecond() != _oldSecond) { // One second has passed, reset millis offset and second
        _oldSecond = nmea.getSecond();
        _oldMillis = curMillis;
    }
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",nmea.getYear(), nmea.getMonth(), nmea.getDay(), nmea.getHour(), nmea.getMinute(), nmea.getSecond(), curMillis-_oldMillis);
}


// =========================
// === HANDLER FUNCTIONS ===
// =========================


void ppsHandler(void) {
    ppsTriggered = true;
}

void printUnknownSentence(MicroNMEA &nmea, Stream &out) {
    out.println();
	out.print("Unknown sentence: ");
	out.println(nmea.getSentence());
}