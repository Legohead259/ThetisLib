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
    GPS.begin(38400); // Begin talking with GPS at default 38400 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!GPS) {
        diagLogger->fatal("Failed to initialize GPS!");
        return false;
    }
    delay(10);
    diagLogger->trace("Setting GPS baud rate to 9600 BPS");
    nmea.setUnknownSentenceHandler(printUnknownSentence);
    MicroNMEA::sendSentence(GPS, "$PMTK251,9600");         // Set GPS baudrate to 38400
    delay(10);
    GPS.begin(9600);
    diagLogger->trace("Enabling NMEA RMC and GGA strings");
    MicroNMEA::sendSentence(GPS, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    diagLogger->trace("Setting GPS update rate to 1 Hz");
    MicroNMEA::sendSentence(GPS, "$PMTK220,1000");           // Set GPS update rate to 100 ms (10 Hz)
    diagLogger->info("done!");

    return true;
}

void pollGPS() {    
    while (GPS.available()) {
        char c = GPS.read();
        nmea.process(c);
    }
    diagLogger->trace("GPS Sentence: %s", nmea.getSentence());

    char _buf[64];
    getISO8601Time_GPS(_buf);
    diagLogger->trace("GPS Date: %s", _buf);
    diagLogger->trace("GPS Fix:  %s \t Sats: %d \t HDOP: %d", nmea.isValid() ? "true" : "false", nmea.getNumSatellites(), nmea.getHDOP());
    diagLogger->trace("GPS Lat:  %0.3f \t Lon: %0.3f \t Speed: %0.3f \t Course %0.3f", (float) nmea.getLatitude()/1E6, (float) nmea.getLongitude()/1E6, (float) nmea.getSpeed()/1E3, (float) nmea.getCourse()/1E3);
}

// =========================
// === HANDLER FUNCTIONS ===
// =========================


void ppsHandler(void) {
    ppsTriggered = true;
}

void printUnknownSentence(MicroNMEA &nmea) {
    diagLogger->warn("Unknown sentence: %s", nmea.getSentence());
}