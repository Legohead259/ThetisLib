#ifndef RTC_H
#define RTC_H

#include <TimeLib.h>
#include "../filesystem/logger.h"
#include "../radios/gps.h"

extern tmElements_t espRTCTime;

void syncInternalClock(const char* timeStr);
void syncInternalClockGPS();
void updateTimestamp();
long updateRTCms();
void getISO8601Time_GPS(char *buf);
void getISO8601Time_RTC(char *buf);
void getTime_RTC(char *buf);


#endif // RTC_H