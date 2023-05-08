#ifndef IMU_H
#define IMU_H

#include "../sensors/lsm6dso32.h"
#ifdef MAG_ENABLE
#include "../sensors/lis3mdl.h"
#endif // REV_F5 || REV_G2
// #include "MahonyAHRS.h"
#include "MadgwickAHRS.h"
#include "../utility/imumaths.h"
#include "../data.h"
#include "../filesystem/logger.h"

uint8_t fusionUpdateRate = 200; // Hz - Default: 200
unsigned long fusionUpdateInterval = 1000/fusionUpdateRate; // time between IMU polls [ms]

// Mahony mahony(fusionUpdateRate);

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp;

sensors_vec_t linAccel;

void initFusion() {
    diagLogger->info("Initializing fusion algorithm...");
    diagLogger->info("done!");
}

void calcLinAccel() {
    // Gravitational acceleration in NED coordinate system
    imu::Vector<3> gravGlobal = {0, 0, 9.81};
    // imu::Quaternion Qb = mahony.getQuaternion();
    imu::Quaternion Qb = getQuaternion();
    imu::Quaternion gravBody = Qb.invert() * imu::Quaternion(0, gravGlobal) * Qb;

    data.linAccelX = data.accelX - gravBody.x();
    data.linAccelY = data.accelY - gravBody.y();
    data.linAccelZ = data.accelZ - gravBody.z();

    diagLogger->trace("LinAccel   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.linAccelX, data.linAccelY, data.linAccelZ);
}

void updateFusion() {
    
    // ----------------------------------
    // -- Update measurements from IMU --
    // ----------------------------------

    pollDSO32();
    #ifdef MAG_ENABLE
    pollLIS3MDL();
    #endif

    // Insert raw measurement readings into data structure
    data.accelX = accel.acceleration.x;
    data.accelY = accel.acceleration.y;
    data.accelZ = accel.acceleration.z;
    data.gyroX = gyro.gyro.x;
    data.gyroY = gyro.gyro.y;
    data.gyroZ = gyro.gyro.z;
    #ifdef MAG_ENABLE
    data.magX = mag.magnetic.x;
    data.magX = mag.magnetic.y;
    data.magX = mag.magnetic.x;
    #endif

    diagLogger->trace("Accel   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.accelX, data.accelY, data.accelZ);
    diagLogger->trace("Gyro    X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.gyroX, data.gyroY, data.gyroZ);
    #ifdef MAG_ENABLE
    diagLogger->trace("Mag     X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.magX, data.magY, data.magZ);
    #endif

    // --------------------------
    // -- Update Mahony Filter --
    // --------------------------

    #ifdef MAG_ENABLE
    // mahony.update(  data.gyroX, data.gyroY, data.gyroZ,
    //                 data.accelX, data.accelY, data.accelZ,
    //                 data.magX, data.magY, data.magZ);
    MadgwickAHRSupdate(   data.gyroX, data.gyroY, data.gyroZ,
                                    data.accelX, data.accelY, data.accelZ,
                                    data.magX, data.magY, data.magZ);
    #else
    mahony.updateIMU(   data.gyroX, data.gyroY, data.gyroZ,
                        data.accelX, data.accelY, data.accelZ);
    #endif
    
    float _quat[4];
    getQuaternionComps(_quat);
    data.quatW = _quat[0];
    data.quatX = _quat[1];
    data.quatY = _quat[2];
    data.quatZ = _quat[3];

    diagLogger->trace("Quaternion W: %0.3f \t X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.quatW, data.quatX, data.quatY, data.quatZ);
    diagLogger->debug("Euler      R: %0.3f \t P: %0.3f \t Y: %0.3f", getRoll(), getPitch(), getYaw());

    // ------------------------------------
    // -- Calculate linear accelerations --
    // ------------------------------------

    calcLinAccel();
}

#endif // IMU_H