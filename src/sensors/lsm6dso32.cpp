#include "lsm6dso32.h"

// ===========================
// === LSM6DSO32 FUNCTIONS ===
// ===========================


Adafruit_LSM6DSO32 dso32;

bool initDSO32( lsm6dso32_accel_range_t accelRange, 
                lsm6ds_gyro_range_t gyroRange,
                lsm6ds_data_rate_t dataRate) {
    #ifdef LSM6DSO_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing DSO32 IMU...");
    #endif
    if (!dso32.begin_I2C(0x6B)) {
        #ifdef LSM6DSO_DEBUG
        DEBUG_SERIAL_PORT.println("Failed to find LSM6DSO32 chip");
        #endif
        return false;
    }
    else {
        dso32.setAccelRange(accelRange);
        dso32.setGyroRange(gyroRange);
        dso32.setAccelDataRate(dataRate);
        dso32.setGyroDataRate(dataRate);
        #ifdef LSM6DSO_DEBUG
        DEBUG_SERIAL_PORT.println("done!");
        #endif
        return true;
    }
}

void pollDSO32() {
    dso32.getEvent(&accel, &gyro, &temp);

    // Convert gyro to deg/s from rad/s
    gyro.gyro.x *= RAD_TO_DEG;
    gyro.gyro.y *= RAD_TO_DEG;
    gyro.gyro.z *= RAD_TO_DEG;

    // Debug print statements
    #ifdef LSM6DSO_DEBUG
    DEBUG_SERIAL_PORT.printf("Accel X: %0.3f \tY: %0.3f \tZ: %0.3f m/s/s\n\r", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
    DEBUG_SERIAL_PORT.printf(" Gyro X: %0.3f \tY: %0.3f \tZ: %0.3f rad/s\n\r", gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    DEBUG_SERIAL_PORT.printf("Temperature: %0.3f °C\n\n\r", temp.temperature);
    #endif // LSM6DSO_DEBUG

    #ifdef LSM6DSO_DEBUG_PLOTTER
    // Serial plotter print statements
    DEBUG_SERIAL_PORT.printf("%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n\r",data.accelX, data.accelY, data.accelZ, data.linAccelX, data.linAccelY, data.linAccelZ);
    #endif // LSM6DSO_DEBUG_PLOTTER
}

// =========================
// === UTILITY FUNCTIONS === 
// =========================

lsm6dso32_accel_range_t getAccelRange(uint8_t range) {
    switch (range) {
        case 4:
            return LSM6DSO32_ACCEL_RANGE_4_G;
        case 8:
            return LSM6DSO32_ACCEL_RANGE_8_G;
        case 16:
            return LSM6DSO32_ACCEL_RANGE_16_G;
        default:
            return LSM6DSO32_ACCEL_RANGE_4_G;
    }
}

lsm6ds_gyro_range_t getGyroRange(uint16_t range) {
    switch (range) {
        case 125:
            return LSM6DS_GYRO_RANGE_125_DPS;
        case 250:
            return LSM6DS_GYRO_RANGE_250_DPS;
        case 500:
            return LSM6DS_GYRO_RANGE_500_DPS;
        case 1000:
            return LSM6DS_GYRO_RANGE_1000_DPS;
        case 2000:
            return LSM6DS_GYRO_RANGE_2000_DPS;
        default:
            return LSM6DS_GYRO_RANGE_125_DPS;
    }
}

lsm6ds_data_rate_t getDataRate(double rate) {
    switch ((int) rate*10) {
        case 0:
            return LSM6DS_RATE_SHUTDOWN;
        case 125:
            return LSM6DS_RATE_12_5_HZ;
        case 260:
            return LSM6DS_RATE_26_HZ;
        case 520:
            return LSM6DS_RATE_52_HZ;
        case 1040:
            return LSM6DS_RATE_104_HZ;
        case 2080:
            return LSM6DS_RATE_208_HZ;
        case 4160:
            return LSM6DS_RATE_416_HZ;
        case 8330:
            return LSM6DS_RATE_833_HZ;
        case 16600:
            return LSM6DS_RATE_1_66K_HZ;
        case 33300:
            return LSM6DS_RATE_3_33K_HZ;
        case 66600:
            return LSM6DS_RATE_6_66K_HZ;
        default:
            return LSM6DS_RATE_SHUTDOWN; 
    }
}

double getDataRate(lsm6ds_data_rate_t rate) {
    switch (rate) {
        case LSM6DS_RATE_SHUTDOWN:
            return 0;
        case LSM6DS_RATE_12_5_HZ:
            return 12.5;
        case LSM6DS_RATE_26_HZ:
            return 26;
        case LSM6DS_RATE_52_HZ:
            return 52;
        case LSM6DS_RATE_104_HZ:
            return 104;
        case LSM6DS_RATE_208_HZ:
            return 208;
        case LSM6DS_RATE_416_HZ:
            return 416;
        case LSM6DS_RATE_833_HZ:
            return 833;
        case LSM6DS_RATE_1_66K_HZ:
            return 1660;
        case LSM6DS_RATE_3_33K_HZ:
            return 3330;
        case LSM6DS_RATE_6_66K_HZ:
            return 6660;
        default:
            return 208;
    }
}
