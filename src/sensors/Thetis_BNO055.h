#ifndef THETIS_BNO055_H
#define THETIS_BNO055_H


// ========================
// === BNO055 FUNCTIONS ===
// ========================


#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>

bool initBNO055(Adafruit_BNO055 &imu, Stream &out);

#endif // THETIS_BNO055_H