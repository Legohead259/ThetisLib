#ifndef IMU_H
#define IMU_H

#include "../sensors/lsm6dso32.h"
#if defined(REV_F5) || defined(REV_G2)
#include "../sensors/lis3mdl.h"
#endif // REV_F5 || REV_G2
#include "MahonyAHRS.h"
#include "../utility/imumaths.h"
#include "../data.h"
#include "../filesystem/logger.h"

uint8_t fusionUpdateRate = 200; // Hz - Default: 200
unsigned long fusionUpdateInterval = 1000/fusionUpdateRate; // time between IMU polls [ms]

Mahony mahony(fusionUpdateRate);

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp;

sensors_vec_t linAccel;
sensors_vec_t accelAngle;
sensors_vec_t ufAngle;
sensors_vec_t cfAngle;
sensors_vec_t kfAngle;
sensors_vec_t eulerAngles;

// Kalman filter instantiation
#include <Kalman.h>

#define N_state 9 // Number of states - Acceleration (XYZ), Gyroscope (XYZ), Magnetometer (XYZ)
#define N_obs 	9 // Number of observation - Acceleration (XYZ), Gyroscope (XYZ), Magnetometer (XYZ)
#define r_a 	0.5 // Acceleration measurement noise (m/sec/sec)
#define r_g 	1.0 // Gyroscope measurement noise (deg/sec)
#define r_m     1.0 // Magnetometer measurement noise (uT)
#define q_a 	0.1 // Process error - acceleration
#define q_g 	0.1 // Process error - gyroscope
#define q_m     0.1 // Process error - magnetometer

BLA::Matrix<N_obs> obs; // observation vector
KALMAN<N_state, N_obs> K_imu; // Kalman filter object
unsigned long currMillis;
float dt;

void initFusion() {
    diagLogger->info("Initializing fusion algorithm...");
    // Initialize Kalman Filter

	// time evolution matrix (whatever... it will be updated in loop)
    //          ax   ay   az   gx   gy   gz   mx   my   mz
    K_imu.F = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};

    // measurement matrix
    K_imu.H = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    
    // measurement covariance matrix
	float r_a2 = r_a*r_a;
	float r_g2 = r_g*r_g;
    float r_m2 = r_m*r_m;
    K_imu.R = { r_a2, 	0.0,    0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0, 
                0.0,    r_a2, 	0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,    0.0,    r_a2,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,    0.0,	0.0, 	r_g2, 	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	r_g2, 	0.0,    0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	r_g2,   0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    r_m2,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    0.0,	r_m2,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	r_m2,};
    diagLogger->verbose("Accelerometer variance set to: %d m/s/s", r_a);
    diagLogger->verbose("Gyroscope variance set to: %d deg/s", r_g);
    diagLogger->verbose("Magnetometer variance set to: %d uT", r_m);
    
    // model covariance matrix
	float q_a2 = q_a*q_a;
	float q_g2 = q_g*q_g;
    float q_m2 = q_m*q_m;
    K_imu.Q = { q_a2, 	0.0,    0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0, 
                0.0,    q_a2, 	0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,    0.0,    q_a2,	0.0,	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,    0.0,	0.0, 	q_g2, 	0.0,	0.0,    0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	q_g2, 	0.0,    0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	q_g2,   0.0,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    q_m2,	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    0.0,	q_m2,	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	0.0,    0.0,	0.0,	q_m2,};
    diagLogger->verbose("Accelerometer process noise set to: %d", q_a);
    diagLogger->verbose("Gyroscope process noise set to: %d", q_g);
    diagLogger->verbose("Magnetometer process noise set to: %d", q_m);
    
    currMillis = millis();
    diagLogger->info("done!");
}

void calcLinAccel() {
    // Gravitational acceleration in NED coordinate system
    imu::Vector<3> gravGlobal = {0, 0, 9.81};
    imu::Quaternion Qb = mahony.getQuaternion();
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
    #if defined(REV_F5) || defined(REV_G2)
    pollLIS3MDL();
    #endif

    // Insert raw measurement readings into data structure
    data.rawAccelX = accel.acceleration.x;
    data.rawAccelY = accel.acceleration.y;
    data.rawAccelZ = accel.acceleration.z;
    data.rawGyroX = gyro.gyro.x;
    data.rawGyroY = gyro.gyro.y;
    data.rawGyroZ = gyro.gyro.z;
    #if defined(REV_F5) || defined(REV_G2)
    data.rawMagX = mag.magnetic.x;
    data.rawMagX = mag.magnetic.y;
    data.rawMagX = mag.magnetic.x;
    #endif

    diagLogger->trace("RawAccel   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.rawAccelX, data.rawAccelY, data.rawAccelZ);
    diagLogger->trace("RawGyro    X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.rawGyroX, data.rawGyroY, data.rawGyroZ);
    #if defined(REV_F5) || defined(REV_G2)
    diagLogger->trace("RawMag     X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.rawMagX, data.rawMagY, data.rawMagZ);
    #endif

    // --------------------------
    // -- Update Kalman Filter --
    // --------------------------

    // Create measurement matrix
    BLA::Matrix<N_obs> meas;
    meas(0) = accel.acceleration.x;
    meas(1) = accel.acceleration.y;
    meas(2) = accel.acceleration.z;
	meas(3) = gyro.gyro.x;
	meas(4) = gyro.gyro.y;
	meas(5) = gyro.gyro.z;
    #if defined(REV_F5) || defined(REV_G2)
    meas(6) = mag.magnetic.x;
    meas(7) = mag.magnetic.y;
    meas(8) = mag.magnetic.z;
    #endif
    obs = K_imu.H * meas;
    
    // Update Kalman filter
    K_imu.update(obs);

    // Update data structure
    data.accelX = K_imu.x(0);
    data.accelY = K_imu.x(1);
    data.accelZ = K_imu.x(2);
    data.gyroX  = K_imu.x(3);
    data.gyroY  = K_imu.x(4);
    data.gyroZ  = K_imu.x(5);
    #if defined(REV_F5) || defined(REV_G2)
    data.magX = K_imu.x(6);
    data.magY = K_imu.x(7);
    data.magZ = K_imu.x(8);
    #endif

    diagLogger->trace("Accel      X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.accelX, data.accelY, data.accelZ);
    diagLogger->trace("Gyroscope  X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.gyroX, data.gyroY, data.gyroZ);
    diagLogger->trace("Magnetic   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.magX, data.magY, data.magZ);

    // --------------------------
    // -- Update Mahony Filter --
    // --------------------------

    #if defined(REV_F5) || defined(REV_G2)
    // mahony.update(  data.gyroX, data.gyroY, data.gyroZ,
    //                 data.accelX, data.accelY, data.accelZ,
    //                 data.magX, data.magY, data.magZ);
    mahony.update(  data.rawGyroX, data.rawGyroY, data.rawGyroZ,
                    data.rawAccelX, data.rawAccelY, data.rawAccelZ,
                    data.rawMagX, data.rawMagY, data.rawMagZ);
    #else
    mahony.updateIMU(   data.gyroX, data.gyroY, data.gyroZ,
                        data.accelX, data.accelY, data.accelZ);
    #endif

    data.roll = mahony.getRoll();
    data.pitch = mahony.getPitch();
    data.yaw = mahony.getYaw();

    diagLogger->info("Attitude   R: %0.3f \t P: %0.3f \t Y: %0.3f", data.roll, data.pitch, data.yaw);
    
    float _quat[4];
    mahony.getQuaternionComps(_quat);
    data.quatW = _quat[0];
    data.quatX = _quat[1];
    data.quatY = _quat[2];
    data.quatZ = _quat[3];

    diagLogger->trace("Quaternion W: %0.3f \t X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.quatW, data.quatX, data.quatY, data.quatZ);

    // ------------------------------------
    // -- Calculate linear accelerations --
    // ------------------------------------

    calcLinAccel();
}

#endif // IMU_H