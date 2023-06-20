#include "rtc.h"

tmElements_t espRTCTime;

void syncInternalClock(const char* timeStr) {
    tm timeStruct;
    sscanf(timeStr, "%d-%d-%d %d:%d:%d",    &timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday, 
                                            &timeStruct.tm_hour, &timeStruct.tm_min, &timeStruct.tm_sec);
    timeStruct.tm_year -= 1900;
    timeStruct.tm_mon -= 1;

    setTime(mktime(&timeStruct));
}

void syncInternalClockGPS() {
    diagLogger->info("Attempting to sync internal RTC to GPS...");

    if (data.GPSFix) { // If the GPS has a good fix, reset the internal clock to the GPS time
        espRTCTime.Year = nmea.getYear()-1970;
        espRTCTime.Month = nmea.getMonth();
        espRTCTime.Day = nmea.getDay();
        espRTCTime.Hour = nmea.getHour();
        espRTCTime.Minute = nmea.getMinute();
        espRTCTime.Second = nmea.getSecond();

        setTime(makeTime(espRTCTime)); // Reset internal clock
        diagLogger->info("Done!");
    }
    else {
        diagLogger->warn("Failed to synchronize - GPS fix not valid!");
    }
}

void updateTimestamp() {
    data.epoch = now();
    data.mSecond = updateRTCms();
}

long updateRTCms() {
    breakTime(now(), espRTCTime);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (espRTCTime.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = espRTCTime.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }
    return curMSecond;
}

void getISO8601Time_GPS(char *buf) {
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", nmea.getYear(), nmea.getMonth(), nmea.getDay(), nmea.getHour(), nmea.getMinute(), nmea.getSecond(), nmea.getHundredths());
}

void getISO8601Time_RTC(char *buf) {
    // Format timestamp into ISO8601 format
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", espRTCTime.Year+1970, espRTCTime.Month, espRTCTime.Day, espRTCTime.Hour, espRTCTime.Minute, espRTCTime.Second, updateRTCms());
}

void getTime_RTC(char* buf) {
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", espRTCTime.Year+1970, espRTCTime.Month, espRTCTime.Day, espRTCTime.Hour, espRTCTime.Minute, espRTCTime.Second);
}