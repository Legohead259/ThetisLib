#ifndef RTC_H
#define RTC_H

#include <TimeLib.h>
#include "../filesystem/logger.h"
#include "../radios/gps.h"
#include "../subsystem.h"
#include "../settings.h"
#include "../timing/src/TimerEvents.h"

class ThetisClock : public ThetisSubsystem {

public:
    ThetisClock() :
    gpsRTCSyncEvent("GPS RTC Sync Event", 60000, synchronizeGPS, getSetting<bool>("gpsRtcSyncEnabled")),
    updateRTCmsEvent("RTC Millisecond Update Event", 1, updateRTCms, true)
    { return; }

    bool begin() override;
    void poll() override {}
    void updateSettings() override {}
    void test() override {}

    static void synchronizeClock(const char* timeStr);
    static void synchronizeGPS();
    void updateTimestamp();
    static long updateRTCms();
    static void getISO8601Time_GPS(char *buf, MicroNMEA& gps);
    static size_t getISO8601Time_RTC(char *buf);
    static size_t getTime_RTC(char *buf);

private:
    tmElements_t espRTCTime;

    TimerEvent gpsRTCSyncEvent;
    TimerEvent updateRTCmsEvent;
};

extern ThetisClock rtc;

#endif // RTC_H