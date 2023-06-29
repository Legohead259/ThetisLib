#ifndef THETIS_LSM6DSO32_H
#define THETIS_LSM6DSO32_H

#include <Adafruit_LSM6DSO32.h>
#include "../filesystem/logger.h"
#include "../subsystem.h"
#include "../settings.h"
#include "../fusion/Fusion/Fusion.h"

class ThetisIMU : Adafruit_LSM6DSO32, ThetisSubsystem {
public:
    FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
    FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
    FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
    FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
    FusionVector gyroscopeData;
    FusionVector accelerometerData;
    FusionOffset gyroOffset;

    bool begin() override;
    void poll() override;
    void updateSettings() override;

    lsm6dso32_accel_range_t getAccelRange(uint8_t range);
    lsm6ds_gyro_range_t getGyroRange(uint16_t range);
    lsm6ds_data_rate_t getDataRate(double rate);
    double getDataRate(lsm6ds_data_rate_t rate);
    double getSampleFrequency(lsm6ds_data_rate_t dataRate);

private:
    lsm6dso32_accel_range_t accelRange;
    lsm6ds_gyro_range_t gyroRange;
    lsm6ds_data_rate_t dataRate;
};

extern ThetisIMU imu;

#endif // ndef THETIS_LSM6DSO32_H