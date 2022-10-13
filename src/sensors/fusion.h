#ifndef IMU_H
#define IMU_H

#include "lsm6dso32.h"
#include "../AHRS/MahonyAHRS.h"
#include "../utility/imumaths.h"
#include "../data.h"

unsigned int imuPollRate = 52.0; // Hz
unsigned long imuPollInterval = 1000/imuPollRate; // time between IMU polls [ms]

Mahony mahony(imuPollRate);

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
#define r_a 	0.5 // Acceleration measurement noise (m/sec)
#define r_g 	1.0 // Gyroscope measurement noise (deg/sec)
#define q_a 	0.1 // Process error - acceleration
#define q_g 	0.1 // Process error - gyroscope

BLA::Matrix<N_obs> obs; // observation vector
KALMAN<N_state, N_obs> K_imu; // Kalman filter object
unsigned long currMillis;
float dt;

void initFusion() {
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
    
    // model covariance matrix
	float q_a2 = q_a*q_a;
	float q_g2 = q_g*q_g;
    K_imu.Q = { q_a2, 	0.0,    0.0,	0.0,	0.0,	0.0,
                0.0,    q_a2, 	0.0,	0.0,	0.0,	0.0,
                0.0,    0.0,    q_a2,	0.0,	0.0,	0.0,
                0.0,    0.0,	0.0, 	q_g2, 	0.0,	0.0,
                0.0,	0.0,	0.0,	0.0,	q_g2, 	0.0,
                0.0,	0.0,	0.0,	0.0,	0.0,	q_g2 };
    
    currMillis = millis();
}

void calcLinAccel() {
    // Gravitational acceleration in NED coordinate system
    imu::Vector<3> gravGlobal = {0, 0, 9.81};
    imu::Quaternion Qb = mahony.getQuaternion();
    imu::Quaternion gravBody = Qb.invert() * imu::Quaternion(0, gravGlobal) * Qb;

    data.linAccelX = data.accelX - gravBody.x();
    data.linAccelY = data.accelY - gravBody.y();
    data.linAccelZ = data.accelZ - gravBody.z();

    // DEBUG statement
    #ifdef IMU_DEBUG
    DEBUG_SERIAL_PORT.printf("LinAccel X: %0.3f \t\t Y: %0.3f \t\t Z: %0.3f \t\t m/s/s\n\r", data.accelX, data.accelY, data.accelZ);
    #endif
}

void updateFusion() {
    
    // ----------------------------------
    // -- Update measurements from IMU --
    // ----------------------------------

    pollDSO32();

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

    #ifdef KALMAN_IMU_DEBUG
    DEBUG_SERIAL_PORT.printf("Acceleration X: %0.3f | %0.3f \n\r", K_imu.x(0), data.accelX);
    DEBUG_SERIAL_PORT.printf("Acceleration Y: %0.3f | %0.3f \n\r", K_imu.x(1), data.accelY);
    DEBUG_SERIAL_PORT.printf("Acceleration Z: %0.3f | %0.3f \n\r", K_imu.x(2), data.accelZ);
    DEBUG_SERIAL_PORT.printf("Gyroscope    X: %0.3f | %0.3f \n\r", K_imu.x(3), data.gyroX);
    DEBUG_SERIAL_PORT.printf("Gyroscope    Y: %0.3f | %0.3f \n\r", K_imu.x(4), data.gyroY);
    DEBUG_SERIAL_PORT.printf("Gyroscope    Z: %0.3f | %0.3f \n\r", K_imu.x(5), data.gyroZ);
    #endif
    DEBUG_SERIAL_PORT.println((int) &data, HEX);

    // --------------------------
    // -- Update Mahony Filter --
    // --------------------------

    mahony.updateIMU(data.gyroX, data.gyroY, data.gyroZ,
                    data.accelX, data.accelY, data.accelZ);

    // ------------------------------------
    // -- Calculate linear accelerations --
    // ------------------------------------

    calcLinAccel();
}

#endif // IMU_H