#include "Thetis_BNO055.h"

bool initBNO055(Adafruit_BNO055 &imu, Stream &out) {
    out.print("Initializing BNO055...");
    digitalWrite(BNO055_RESET, HIGH); // Ensure BNO Reset is HIGH
    if (!imu.begin()) {
        out.println("Failed to initialize BNO055");
        return false;
    }
    else {
        imu.setExtCrystalUse(true);
        out.println("done!");
        return true;
    }
}