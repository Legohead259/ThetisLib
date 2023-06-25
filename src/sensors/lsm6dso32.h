#ifndef THETIS_LSM6DSO32_H
#define THETIS_LSM6DSO32_H

#include <Adafruit_LSM6DSO32.h>
#include "../filesystem/logger.h"
#include "../subsystem.h"
#include "../settings.h"

class ThetisIMU : Adafruit_LSM6DSO32, ThetisSubsystem {
public:
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

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

#endif // ndef THETIS_LSM6DSO32_H