#ifndef DATA_H
#define DATA_H

#include <TimeLib.h>
#include <Arduino.h>

struct data_t {
    time_t epoch;               // Timestamp in seconds since UNIX epoch (January 1, 1970)
    uint16_t GPSYear;           // Current year from GPS data
    uint8_t GPSMonth;           // Current month from GPS data
    uint8_t GPSDay;             // Current day from GPS data
    uint8_t GPSHour;            // Current hour from GPS data
    uint8_t GPSMinute;          // Current minute from GPS data
    uint8_t GPSSecond;         // Current year from GPS data
    uint8_t GPSHundreth;       // Current fraction of second of reading
    float voltage;              // Battery voltage in volts
    bool GPSFix;                // If GPS has positive fix on location
    uint8_t numSats;            // Number of satellites GPS is communicating with
    uint8_t HDOP;               // Accuracy of GPS reading. Lower is better. In tenths (divide by 10. when displaying)
    long latitude;              // In millionths of a degree (divide by 1000000. when displaying)
    long longitude;             // In millionths of a degree (divide by 1000000. when displaying)
    long GPSSpeed;              // In thousandths of a knot (divide by 1000. when displaying)
    long GPSCourse;             // In thousandths of a degree (divide by 1000. when displaying)
    uint8_t sysCal = 0;         // IMU system calibration, 0-3 with 3 being fully calibrated
    uint8_t gyroCal = 0;        // IMU gyroscope calibration, 0-3 with 3 being fully calibrated
    uint8_t accelCal = 0;       // IMU accelerometer calibration, 0-3 with 3 being fully calibrated
    uint8_t magCal = 0;         // IMU magnetometer calibration, 0-3 with 3 being fully calibrated
    float rawAccelX;            // m/s^2
    float rawAccelY;            // m/s^2
    float rawAccelZ;            // m/s^2
    float accelX;               // m/s^2 - filtered through a Kalman filter
    float accelY;               // m/s^2 - filtered through a Kalman filter
    float accelZ;               // m/s^2 - filtered through a Kalman filter
    float rawGyroX;             // rad/s
    float rawGyroY;             // rad/s
    float rawGyroZ;             // rad/s
    float gyroX;                // rad/s - filtered through a Kalman filter
    float gyroY;                // rad/s - filtered through a Kalman filter
    float gyroZ;                // rad/s - filtered through a Kalman filter
    float rawMagX;              // mGauss
    float rawMagY;              // mGauss
    float rawMagZ;              // mGauss
    float magX;                 // mGauss - filtered through a Kalman filter
    float magY;                 // mGauss - filtered through a Kalman filter
    float magZ;                 // mGauss - filtered through a Kalman filter
    float roll;                 // degrees
    float pitch;                // degrees
    float yaw;                  // degrees
    float linAccelX;            // m/s^2
    float linAccelY;            // m/s^2
    float linAccelZ;            // m/s^2
    float quatW;                //
    float quatX;                //
    float quatY;                //
    float quatZ;                //
    float imuTemp;              // °Celsius from the IMU
    uint8_t state;              // State reported by the package.
};

extern data_t data;

#endif // DATA_H