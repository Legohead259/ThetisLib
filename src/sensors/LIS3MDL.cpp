#include "lis3mdl.h"
#include "../filesystem/logger.h"

Adafruit_LIS3MDL lis3mdl;

sensors_event_t mag;

// ===========================
// === LIS3MDL FUNCTIONS ===
// ===========================


bool initLIS3MDL(   lis3mdl_performancemode_t perfMode,
                    lis3mdl_operationmode_t opMode,
                    lis3mdl_dataRate_t dataRate,
                    lis3mdl_range_t range) {
    diagLogger->info("Initializing LIS3MDL...");
    if (!lis3mdl.begin_I2C(0x1E)) {
        diagLogger->fatal("Failed to initialize LIS3MDL!");
        return false;
    }
    else {
        diagLogger->verbose("Setting LIS3MDL performance mode to: %d", perfMode);
        lis3mdl.setPerformanceMode(perfMode);
        diagLogger->verbose("Setting LIS3MDL operation mode to: %d", opMode);
        lis3mdl.setOperationMode(opMode);
        diagLogger->verbose("Setting LIS3MDL data rate to: %d", dataRate);
        lis3mdl.setDataRate(dataRate);
        diagLogger->verbose("Setting LIS3MDL range to: %d", range);
        lis3mdl.setRange(range);
        diagLogger->info("Done!");
        return true;
    }
}

void pollLIS3MDL() {
    lis3mdl.getEvent(&mag); // Update 

    data.magX = mag.magnetic.x;
    data.magY = mag.magnetic.y;
    data.magZ = mag.magnetic.z;
}