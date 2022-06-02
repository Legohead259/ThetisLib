#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "0.1.0"

#include <Arduino.h>


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

bool initLSM6DSO32(Adafruit_LSM6DSO32 imu, Stream &out, 
                    lsm6dso32_accel_range_t accelRange=LSM6DSO32_ACCEL_RANGE_8_G, 
                    lsm6ds_gyro_range_t gyroRange=LSM6DS_GYRO_RANGE_2000_DPS,
                    lsm6ds_data_rate_t dataRate=LSM6DS_RATE_52_HZ);


// ==============================
// === FILE SYSTEMS FUNCTIONS ===
// ==============================


#include "FS.h"
#include <SD.h>
#include <SPI.h>

bool initFS(fs::FS &fs, Stream &out);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels, Stream &out);
bool createDir(fs::FS &fs, const char * path, Stream &out);
bool removeDir(fs::FS &fs, const char * path, Stream &out);
bool readFile(fs::FS &fs, const char * path, Stream &out);
bool writeFile(fs::FS &fs, const char * path, const char * message, Stream &out);
bool appendFile(fs::FS &fs, const char * path, const char * message, Stream &out);
bool renameFile(fs::FS &fs, const char * path1, const char * path2, Stream &out);
bool deleteFile(fs::FS &fs, const char * path, Stream &out);
void testFileIO(fs::FS &fs, const char * path, Stream &out);


// =====================
// === GPS FUNCTIONS ===
// =====================


#include <MicroNMEA.h>

bool initGPS(HardwareSerial &GPS, Stream &out);

#endif