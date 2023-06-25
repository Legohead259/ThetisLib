#include "lis3mdl.h"

bool ThetisMag::begin() {
    diagLogger->info("Initializing LIS3MDL...");
    if (!begin_I2C(0x1E)) {
        diagLogger->fatal("Failed to initialize LIS3MDL!");
        return false;
    }
    diagLogger->verbose("Setting LIS3MDL performance mode to: %d", thetisSettings.magnetometerPerformanceMode);
    setPerformanceMode((lis3mdl_performancemode_t) thetisSettings.magnetometerPerformanceMode);
    diagLogger->verbose("Setting LIS3MDL operation mode to: %d", thetisSettings.magnetometerOperationMode);
    setOperationMode((lis3mdl_operationmode_t) thetisSettings.magnetometerOperationMode);
    diagLogger->verbose("Setting LIS3MDL data rate to: %d Hz", thetisSettings.magnetometerDataRate);
    setDataRate((lis3mdl_dataRate_t) thetisSettings.magnetometerDataRate);
    diagLogger->verbose("Setting LIS3MDL range to: %d Gauss", thetisSettings.magnetometerRange);
    setRange((lis3mdl_range_t) thetisSettings.magnetometerRange);
    diagLogger->info("done!");
    return true;
}

void ThetisMag::poll() {
    getEvent(&readings); // Update 
}