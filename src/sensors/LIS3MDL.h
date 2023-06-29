#ifndef LIS3MDL_H
#define LIS3MDL

#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include "../settings.h"
#include "../subsystem.h"
#include "../filesystem/logger.h"
#include "../fusion/Fusion/Fusion.h"

class ThetisMag : Adafruit_LIS3MDL, ThetisSubsystem {
public:
    FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};
    FusionVector magnetometerData;

    bool begin() override;
    void poll() override;
    void updateSettings() override;

private:
    lis3mdl_performancemode_t performanceMode;
    lis3mdl_operationmode_t operationMode;
    lis3mdl_dataRate_t dataRate;
    lis3mdl_range_t range;
};

extern ThetisMag mag;

#endif // LIS3MDL_H