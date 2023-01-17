#ifndef LIS3MDL_H
#define LIS3MDL

#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>

extern Adafruit_LIS3MDL lis3mdl;

extern sensors_event_t mag;

bool initLIS3MDL(   lis3mdl_performancemode_t perfMode=LIS3MDL_MEDIUMMODE,
                    lis3mdl_operationmode_t opMode=LIS3MDL_CONTINUOUSMODE,
                    lis3mdl_dataRate_t dataRate=LIS3MDL_DATARATE_155_HZ,
                    lis3mdl_range_t range=LIS3MDL_RANGE_4_GAUSS);
void pollLIS3MDL();

#endif // LIS3MDL_H