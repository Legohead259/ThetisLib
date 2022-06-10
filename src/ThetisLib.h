#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "0.1.0"

#include <Arduino.h>


// =========================
// === GENERAL FUNCTIONS ===
// =========================


struct Telemetry {
    char timestamp[32];         // Timestamp in UTC obtained from GPS satellites
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
    uint8_t packetSize;         // The size of the telemetry packet. Used as a debug tool for ground station/thetis comms.
};


// ========================
// === BNO055 FUNCTIONS ===
// ========================


#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>

bool initBNO055(Adafruit_BNO055 &imu, Stream &out);


// ===========================
// === LSM6DSO32 FUNCTIONS ===
// ===========================


#include <Adafruit_LSM6DSO32.h>

extern Adafruit_LSM6DSO32 DSO32_IMU;

bool initLSM6DSO32( Adafruit_LSM6DSO32 &imu=DSO32_IMU, 
                    Stream &out=DEBUG_SERIAL_PORT, 
                    lsm6dso32_accel_range_t accelRange=LSM6DSO32_ACCEL_RANGE_8_G, 
                    lsm6ds_gyro_range_t gyroRange=LSM6DS_GYRO_RANGE_250_DPS,
                    lsm6ds_data_rate_t dataRate=LSM6DS_RATE_52_HZ);

void pollLSM6DSO32( Telemetry &data,
                    Adafruit_LSM6DSO32 &imu=DSO32_IMU,
                    Stream &out=DEBUG_SERIAL_PORT);


// ==============================
// === FILE SYSTEMS FUNCTIONS ===
// ==============================


#include "FS.h"
#include <SD.h>
#include <SPI.h>

bool initSDCard(Stream &out=DEBUG_SERIAL_PORT);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels, Stream &out=DEBUG_SERIAL_PORT);
bool createDir(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool removeDir(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool readFile(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool writeFile(fs::FS &fs, const char * path, const char * message, Stream &out=DEBUG_SERIAL_PORT);
bool appendFile(fs::FS &fs, const char * path, const char * message, Stream &out=DEBUG_SERIAL_PORT);
bool renameFile(fs::FS &fs, const char * path1, const char * path2, Stream &out=DEBUG_SERIAL_PORT);
bool deleteFile(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
void testFileIO(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool initLogFile(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
void writeTelemetryData(fs:FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);


// =====================
// === GPS FUNCTIONS ===
// =====================


#include <MicroNMEA.h>

extern HardwareSerial& GPS;
extern char nmeaBuffer[100];
extern MicroNMEA nmea;
extern bool ledState;
extern volatile bool ppsTriggered;

bool initGPS(HardwareSerial &GPS=GPS_SERIAL_PORT, Stream &out=DEBUG_SERIAL_PORT);


// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================

// Neopixel instantiation
#include <Adafruit_NeoPixel.h>
#define DASH_ON 500 // ms
#define DOT_ON 125  // ms
#define BLINK_INTERVAL 125 // ms
#define MESSAGE_INTERVAL 1000 // ms
#define MAXIMUM_BRIGHTNESS 32
#define NUM_STEPS 16
#define BRIGHTNESS_STEP MAXIMUM_BRIGHTNESS/NUM_STEPS

/*
Code Table:
Error       |  DOT  |  DASH  |  DOT  |  DASH  |  CODE  |  COLOR  |  DESCRIPTION
------------|-------|--------|-------|--------|--------|---------|--------------------------------------------------
General     |   0   |   0    |   0   |    1   |  B0001 |   RED   | Unknown, but critical failure
Card Mount  |   0   |   0    |   1   |    0   |  B0010 |   RED   | Filesystem fails to mount
Card Type   |   0   |   0    |   1   |    1   |  B0011 |   RED   | Filesystem initializes, but reports a bad type
File Open   |   0   |   1    |   0   |    0   |  B0100 |   RED   | Datalog file fails to open
Radio       |   0   |   1    |   0   |    1   |  B0101 |   RED   | Radio fails to initialize/encounters error
GPS         |   0   |   1    |   1   |    0   |  B0110 |   RED   | GPS radio fails to initialize
IMU         |   0   |   1    |   1   |    1   |  B0111 |   RED   | IMU fails to initialize
Low Battery |   1   |   0    |   0   |    0   |  B1000 |  AMBER  | Battery voltage is below 3.0V

A dot is DOT_ON on, DOT_ON off
A dash is DASH_ON on, DASH_ON off
Space between codes is MESSAGE_INTERVAL
*/

typedef enum ErrorCode {
    GEN_ERROR_CODE          = B0001,
    CARD_MOUNT_ERROR_CODE   = B0010,
    CARD_TYPE_ERROR_CODE    = B0011,
    FILE_ERROR_CODE         = B0100,
    RADIO_ERROR_CODE        = B0101,
    GPS_ERROR_CODE          = B0110,
    IMU_ERROR_CODE          = B0111,
    LOW_BATT_ERROR_CODE     = B1000
} ErrorCode_t;
 
/*
Status Table:
State           |  Color  |  Indication  |  Description
----------------|---------|--------------|---------------
Logging, No GPS |  BLUE   |    Solid     | Thetis is logging, but does not have a GPS fix
Logging, GPS    |  GREEN  |    Solid     | Thetis is logging with a GPS fix
Ready, No GPS   |  BLUE   |   Pulsing    | Accelerometer is calibrated but no GPS fix
Ready, GPS      |  GREEN  |   Pulsing    | Accelerometer is calibrated and there is a GPS fix
Standby         |  AMBER  |    Solid     | Accelerometer is not calibrated yet
*/

typedef enum Status {
    LOGGING_NO_GPS,
    LOGGING_GPS,
    READY_NO_GPS,
    READY_GPS,
    STANDBY
} Status_t;
extern Status_t currentState;
extern Adafruit_NeoPixel pixel;

const uint32_t OFF      = Adafruit_NeoPixel::Color(0, 0, 0);       // GRB
const uint32_t WHITE    = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BLUE     = Adafruit_NeoPixel::Color(0, 0, 255);
const uint32_t RED      = Adafruit_NeoPixel::Color(0, 255, 0);
const uint32_t GREEN    = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t PURPLE   = Adafruit_NeoPixel::Color(0, 255, 255);
const uint32_t AMBER    = Adafruit_NeoPixel::Color(191, 255, 0);
const uint32_t CYAN     = Adafruit_NeoPixel::Color(255, 0, 255);
const uint32_t LIME     = Adafruit_NeoPixel::Color(125, 0, 255);

const float brightness = 0.1;

void initNeoPixel(Adafruit_NeoPixel &strip=pixel, Stream &out=Serial);
void pulseLED(uint32_t color, Adafruit_NeoPixel &strip=pixel);
void rainbow(Adafruit_NeoPixel &strip=pixel);
uint32_t Wheel(byte wheelPos);
void blinkCode(ErrorCode_t code, uint32_t color=RED, Adafruit_NeoPixel &strip=pixel);


// =========================
// === LOGGING FUNCTIONS ===
// =========================


#define LOG_PRESS_TIME 1000 // ms

extern long logButtonStartTime;
// extern long logButtonPresses;
extern bool logButtonPressed;
extern bool isLogging;

void IRAM_ATTR logButtonISR();

#endif // THETISLIB_H