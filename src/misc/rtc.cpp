#include "rtc.h"

tmElements_t tm;

void syncInternalClockGPS() {
    diagLogger->info("Attempting to sync internal RTC to GPS...");

    if (data.GPSFix) { // If the GPS has a good fix, reset the internal clock to the GPS time
        tm.Year = data.GPSYear-1970;
        tm.Month = data.GPSMonth;
        tm.Day = data.GPSDay;
        tm.Hour = data.GPSHour;
        tm.Minute = data.GPSMinute;
        tm.Second = data.GPSSecond;

        setTime(makeTime(tm)); // Reset internal clock
        diagLogger->info("Done!");
    }
    else {
        diagLogger->warn("Failed to synchronize - GPS fix not valid!");
    }
}

void updateTimestamp() {
    breakTime(now(), tm);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (tm.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = tm.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }
    data.epoch = now();
    data.mSecond = curMSecond;
}

void getISO8601Time_GPS(char *buf) {
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", data.GPSYear, data.GPSMonth, data.GPSDay, data.GPSHour, data.GPSMinute, data.GPSSecond, data.GPSHundredth);
}

void getISO8601Time_RTC(char *buf) {
    breakTime(now(), tm);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (tm.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = tm.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }

    // Format timestamp into ISO8601 format
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, curMSecond);
}