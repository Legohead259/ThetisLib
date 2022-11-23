#include "gps.h"

// ============================
// === GPS MODULE FUNCTIONS ===
// ============================


HardwareSerial& GPS = GPS_SERIAL_PORT;
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
bool ledState = LOW;
volatile bool ppsTriggered = false;

bool initGPS() {
    diagLogger->info("Initializing GPS...");
    GPS.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!GPS) {
        diagLogger->fatal("Failed to initialize GPS!");
        return false;
    }
    diagLogger->info("done!");
    delay(10);
    diagLogger->verbose("Setting GPS baud rate to 38400 BPS");
    MicroNMEA::sendSentence(GPS, "$PMTK251,38400");         // Set GPS baudrate to 38400
    delay(10);
    GPS.begin(38400);
    diagLogger->verbose("Enabling NMEA RMC and GGA strings");
    MicroNMEA::sendSentence(GPS, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    diagLogger->verbose("Setting GPS update rate to 10 Hz");
    MicroNMEA::sendSentence(GPS, "$PMTK220,100");           // Set GPS update rate to 100 ms (10 Hz)

    // TODO: implement a check for good GPS data
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

void pollGPS() {
    while (GPS.available()) {
        char c = GPS.read();
        nmea.process(c);
    }

    //Parse timestamp
    data.GPSYear = nmea.getYear();
    data.GPSMonth = nmea.getMonth();
    data.GPSDay = nmea.getDay();
    data.GPSHour = nmea.getHour();
    data.GPSMinute = nmea.getMinute();
    data.GPSSecond = nmea.getSecond();
    data.GPSHundredth = nmea.getHundredths();

    //Update NMEA string based on PPS pulse from GPS. By default refresh rate is 1Hz
    data.GPSFix = nmea.isValid();
    data.numSats = nmea.getNumSatellites();
    data.HDOP = nmea.getHDOP();
    data.latitude = nmea.getLatitude();
    data.longitude = nmea.getLongitude();
    // nmea.getAltitude(data.altitudeMSL);
    data.GPSSpeed = nmea.getSpeed();
    data.GPSCourse = nmea.getCourse();
}

// =========================
// === HANDLER FUNCTIONS ===
// =========================


void ppsHandler(void) {
    ppsTriggered = true;
}

void printUnknownSentence(MicroNMEA &nmea) {
    DEBUG_SERIAL_PORT.println();
	DEBUG_SERIAL_PORT.print("Unknown sentence: ");
	DEBUG_SERIAL_PORT.println(nmea.getSentence());
}