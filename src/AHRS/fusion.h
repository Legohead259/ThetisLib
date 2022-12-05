#ifndef IMU_H
#define IMU_H

#include "../sensors/lsm6dso32.h"
#ifdef REVF2 || REVG2
#include "../sensors/LIS3MDL.h"
#endif // REVf2 || REVG2
#include "MahonyAHRS.h"
#include "../utility/imumaths.h"
#include "../data.h"
#include "../filesystem/logger.h"

uint8_t fusionUpdateRate = 200; // Hz - Default: 200
unsigned long fusionUpdateInterval = 1000/fusionUpdateRate; // time between IMU polls [ms]

Mahony mahony(fusionUpdateRate);

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

sensors_vec_t linAccel;
sensors_vec_t accelAngle;
sensors_vec_t ufAngle;
sensors_vec_t cfAngle;
sensors_vec_t kfAngle;
sensors_vec_t eulerAngles;

// Kalman filter instantiation
#include <Kalman.h>

#define N_state 6 // Number of states - Acceleration (XYZ), Gyroscope (XYZ)
#define N_obs 	6 // Number of observation - Acceleration (XYZ), Gyroscope (XYZ)
#define r_a 	0.5 // Acceleration measurement noise (m/sec/sec)
#define r_g 	1.0 // Gyroscope measurement noise (deg/sec)
#define r_m     1.0 // Magnetometer measurement noise (nT)
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
    //          ax   ay   az   gx   gy   gz
    K_imu.F = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

    // measurement matrix
    K_imu.H = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 0.0, 0.0, 1.0, 1.0 };
    
    // measurement covariance matrix
	float r_a2 = r_a*r_a;
	float r_g2 = r_g*r_g;
    K_imu.R = { r_a2, 	0.0,    0.0,	0.0,	0.0,	0.0,
                0.0,    r_a2, 	0.0,	0.0,	0.0,	0.0,
                0.0,    0.0,    r_a2,	0.0,	0.0,	0.0,
                0.0,    0.0,	0.0, 	r_g2, 	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	r_g2, 	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	r_g2 };
    diagLogger->verbose("Accelerometer variance set to: %d m/s/s", r_a);
    diagLogger->verbose("Gyroscope variance set to: %d m/s/s", r_g);
    diagLogger->verbose("Magnetometer variance set to: %d m/s/s", r_m);
    
    // model covariance matrix
	float q_a2 = q_a*q_a;
	float q_g2 = q_g*q_g;
    K_imu.Q = { q_a2, 	0.0,    0.0,	0.0,	0.0,	0.0,
                0.0,    q_a2, 	0.0,	0.0,	0.0,	0.0,
                0.0,    0.0,    q_a2,	0.0,	0.0,	0.0,
                0.0,    0.0,	0.0, 	q_g2, 	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	q_g2, 	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	q_g2 };
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

    diagLogger->verbose("LinAccel   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.linAccelX, data.linAccelY, data.linAccelZ);
}

void updateFusion() {
    
    // ----------------------------------
    // -- Update measurements from IMU --
    // ----------------------------------

    pollDSO32();

    // Insert raw measurement readings into data structure
    data.rawAccelX = accel.acceleration.x;
    data.rawAccelY = accel.acceleration.y;
    data.rawAccelZ = accel.acceleration.z;
    data.rawGyroX = gyro.gyro.x;
    data.rawGyroY = gyro.gyro.y;
    data.rawGyroZ = gyro.gyro.z;

    diagLogger->verbose("RawAccel   X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.rawAccelX, data.rawAccelY, data.rawAccelZ);
    diagLogger->verbose("RawGyro    X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.rawGyroX, data.rawGyroY, data.rawGyroZ);


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

    diagLogger->verbose("Accel      X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.accelX, data.accelY, data.accelZ);
    diagLogger->verbose("Gyroscope  X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.gyroX, data.gyroY, data.gyroZ);

    // --------------------------
    // -- Update Mahony Filter --
    // --------------------------

    mahony.updateIMU(data.gyroX, data.gyroY, data.gyroZ,
                    data.accelX, data.accelY, data.accelZ);
    data.roll = mahony.getRoll();
    data.pitch = mahony.getPitch();
    data.yaw = mahony.getYaw();

    diagLogger->verbose("Attitude   R: %0.3f \t P: %0.3f \t Y: %0.3f", data.roll, data.pitch, data.yaw);
    
    float _quat[4];
    mahony.getQuaternionComps(_quat);
    data.quatW = _quat[0];
    data.quatX = _quat[1];
    data.quatY = _quat[2];
    data.quatZ = _quat[3];

    diagLogger->verbose("Quaternion W: %0.3f \t X: %0.3f \t Y: %0.3f \t Z: %0.3f", data.quatW, data.quatX, data.quatY, data.quatZ);

    // ------------------------------------
    // -- Calculate linear accelerations --
    // ------------------------------------

    calcLinAccel();
}

#endif // IMU_H