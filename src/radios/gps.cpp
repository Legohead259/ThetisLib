#include "gps.h"

bool ThetisGPS::begin() {
    diagLogger->info("Initializing GPS...");
    serialPort.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!serialPort) {
        diagLogger->fatal("Failed to initialize GPS!");
        return false;
    }
    diagLogger->trace("Setting GPS baud rate to 9600 BPS");
    setUnknownSentenceHandler(printUnknownSentence);
    sendSentence(serialPort, "$PMTK251,9600");         // Set GPS baudrate to 38400
    delay(10);
    serialPort.begin(9600);
    diagLogger->trace("Enabling NMEA RMC and GGA strings");
    sendSentence(serialPort, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    diagLogger->trace("Setting GPS update rate to 1 Hz");
    sendSentence(serialPort, "$PMTK220,1000");           // Set GPS update rate to 1000 ms (1 Hz)
    diagLogger->info("done!");

    return true;
}

void ThetisGPS::poll() {    
    while (serialPort.available()) {
        char c = serialPort.read();
        process(c);
    }
    diagLogger->trace("GPS Sentence: %s", getSentence());

    char _buf[64];
    getISO8601Time_GPS(_buf);
    diagLogger->trace("GPS Date: %s", _buf);
    diagLogger->trace("GPS Fix:  %s \t Sats: %d \t HDOP: %d", isValid() ? "true" : "false", getNumSatellites(), getHDOP());
    diagLogger->trace("GPS Lat:  %0.3f \t Lon: %0.3f \t Speed: %0.3f \t Course %0.3f", (float) getLatitude()/1E6, (float) getLongitude()/1E6, (float) getSpeed()/1E3, (float) getCourse()/1E3);
}

void ThetisGPS::updateSettings() {

}

void ThetisGPS::getISO8601Time_GPS(char *buf) {
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", getYear(), getMonth(), getDay(), getHour(), getMinute(), getSecond(), getHundredths());
}

ThetisGPS gps;