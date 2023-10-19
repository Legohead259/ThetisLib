#include "rtc.h"

bool ThetisClock::begin() {
    diagLogger->info("Initializing RTC...");
    if (getSetting<bool>("gpsRtcSyncEnabled")) synchronizeGPS(); // Attempt to synchronize RTC to GPS is allowed
    
    // timerEvents
    return true;
}

void ThetisClock::synchronizeGPS() {
    diagLogger->info("Attempting to sync internal RTC to GPS...");
    tmElements_t timeStruct;

    if (gps.isValid()) { // If the GPS has a good fix, reset the internal rtc to the GPS time
        timeStruct.Year = gps.getYear()-1970;
        timeStruct.Month = gps.getMonth();
        timeStruct.Day = gps.getDay();
        timeStruct.Hour = gps.getHour();
        timeStruct.Minute = gps.getMinute();
        timeStruct.Second = gps.getSecond();

        setTime(makeTime(timeStruct)); // Reset internal rtc
        diagLogger->info("done syncing RTC to GPS!");
    }
    else {
        diagLogger->warn("Failed to synchronize - GPS fix not valid!");
    }
}

void ThetisClock::updateTimestamp() {
    // data.epoch = now();
    // data.mSecond = updateRTCms();
    // TODO: Reimplement somehow ?
}

long ThetisClock::updateRTCms() {
    tmElements_t timeStruct;
    breakTime(now(), timeStruct);
    static long _lastSecond = 0;
    static long _lastMSecond = 0;
    long curMSecond = millis();
    if (timeStruct.Second == _lastSecond) {
        curMSecond = millis() - _lastMSecond;
        // Serial.println((int) curMSecond); //DEBUG
    }
    else {
        _lastSecond = timeStruct.Second;
        _lastMSecond = millis();
        curMSecond = 0;
    }
    return curMSecond;
}

size_t ThetisClock::getISO8601Time_RTC(char *buf) {
    // Format timestamp into ISO8601 format
    tmElements_t timeStruct;
    breakTime(now(), timeStruct);
    return sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", timeStruct.Year+1970, timeStruct.Month, timeStruct.Day, timeStruct.Hour, timeStruct.Minute, timeStruct.Second, updateRTCms());
}

size_t ThetisClock::getTime_RTC(char* buf) {
    tmElements_t timeStruct;
    breakTime(now(), timeStruct);
    return sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", timeStruct.Year+1970, timeStruct.Month, timeStruct.Day, timeStruct.Hour, timeStruct.Minute, timeStruct.Second);
}

void ThetisClock::synchronizeClock(const char* timeStr) {
    tm timeStruct;
    sscanf(timeStr, "%d-%d-%d %d:%d:%d",    &timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday, 
                                            &timeStruct.tm_hour, &timeStruct.tm_min, &timeStruct.tm_sec);
    timeStruct.tm_year -= 1900;
    timeStruct.tm_mon -= 1;

    setTime(mktime(&timeStruct));
}

ThetisClock rtc;