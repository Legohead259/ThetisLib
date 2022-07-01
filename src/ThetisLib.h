#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "0.1.0"

#include <Arduino.h>

typedef struct {
    time_t epoch;               // Timestamp in seconds since UNIX epoch (January 1, 1970)
    long curmSecond;            // Current fraction of second of reading
    float voltage;              // Battery voltage in V
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
    float accelX;               // m/s^2
    float accelY;               // m/s^2
    float accelZ;               // m/s^2
    float gyroX;                // rad/s
    float gyroY;                // rad/s
    float gyroZ;                // rad/s
    float magX;                 // mGauss
    float magY;                 // mGauss
    float magZ;                 // mGauss
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
    uint8_t packetSize;         // The size of the telemetry packet
} telemetry_t;

#include <filesystem/ThetisFS.h>

#include<TimeLib.h>
extern tmElements_t tm;
bool writeTelemetryData(fs::FS &fs, const char * path, telemetry_t &data, Stream &out=DEBUG_SERIAL_PORT);
void getISO8601Time_RTC(char *buf);

#include <sensors/Thetis_BNO055.h>
#include <sensors/Thetis_LSM6DSO32.h> 
#include <sensors/Thetis_GPS.h>
#include <misc/Thetis_NeoPixel.h>


// =========================
// === LOGGING FUNCTIONS ===
// =========================


#define LOG_BTN_HOLD_TIME 500 // ms

extern long logButtonStartTime;
// extern long logButtonPresses;
extern bool logButtonPressed;
extern bool isLogging;

void IRAM_ATTR logButtonISR();

#endif // THETISLIB_H