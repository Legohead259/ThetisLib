#ifndef LIS3MDL_H
#define LIS3MDL

#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include "../settings.h"
#include "../subsystem.h"
#include "../filesystem/logger.h"

class ThetisMag : Adafruit_LIS3MDL, ThetisSubsystem {
public:
    sensors_event_t readings;

    bool begin() override;
    void poll() override;
    void updateSettings() override;

private:
    lis3mdl_performancemode_t performanceMode;
    lis3mdl_operationmode_t operationMode;
    lis3mdl_dataRate_t dataRate;
    lis3mdl_range_t range;
};

#endif // LIS3MDL_H