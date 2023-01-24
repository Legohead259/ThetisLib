#ifndef RTC_H
#define RTC_H

#include <TimeLib.h>
#include "../data.h"
#include "../filesystem/logger.h"

extern tmElements_t espRTCTime;

void syncInternalClockGPS();
void updateTimestamp();
long updateRTCms();
void getISO8601Time_GPS(char *buf);
void getISO8601Time_RTC(char *buf);


#endif // RTC_H