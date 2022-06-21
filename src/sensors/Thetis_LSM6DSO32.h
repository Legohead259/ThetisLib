#ifndef THETIS_LSM6DSO32_H
#define THETIS_LSM6DSO32_H

#include <filters/ButterworthBP2.h>
#include <filters/Kalman.h>
#include <AHRS/MahonyAHRS.h>
#include <utility/imumaths.h>

#include <Adafruit_LSM6DSO32.h>

// ===========================
// === LSM6DSO32 FUNCTIONS ===
// ===========================

extern Adafruit_LSM6DSO32 DSO32_IMU;

bool initDSO32( lsm6dso32_accel_range_t accelRange=LSM6DSO32_ACCEL_RANGE_8_G, 
                lsm6ds_gyro_range_t gyroRange=LSM6DS_GYRO_RANGE_250_DPS,
                lsm6ds_data_rate_t dataRate=LSM6DS_RATE_52_HZ,
          Stream &out=DEBUG_SERIAL_PORT);
void pollIMU(Stream &out=DEBUG_SERIAL_PORT);
sensors_vec_t calcLinAccel(Stream &out=DEBUG_SERIAL_PORT);
double setSampleFrequency(lsm6ds_data_rate_t dataRate);

#endif // ndef THETIS_LSM6DSO32_H