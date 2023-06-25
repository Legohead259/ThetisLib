#include "lis3mdl.h"

bool ThetisMag::begin() {
    diagLogger->info("Initializing LIS3MDL...");
    if (!begin_I2C(0x1E)) {
        diagLogger->fatal("Failed to initialize LIS3MDL!");
        return false;
    }
    updateSettings();
    return true;
}

void ThetisMag::poll() {
    getEvent(&readings); // Update 
}

void ThetisMag::updateSettings() {
    performanceMode = (lis3mdl_performancemode_t) thetisSettings.magnetometerPerformanceMode;
    operationMode = (lis3mdl_operationmode_t) thetisSettings.magnetometerOperationMode;
    dataRate = (lis3mdl_dataRate_t) thetisSettings.magnetometerDataRate;
    range = (lis3mdl_range_t) thetisSettings.magnetometerRange;

    diagLogger->verbose("Setting LIS3MDL performance mode to: %d", performanceMode);
    setPerformanceMode(performanceMode);
    diagLogger->verbose("Setting LIS3MDL operation mode to: %d", operationMode);
    setOperationMode(operationMode);
    diagLogger->verbose("Setting LIS3MDL data rate to: %d Hz", dataRate);
    setDataRate(dataRate);
    diagLogger->verbose("Setting LIS3MDL range to: %d Gauss", range);
    setRange(range);
    diagLogger->info("done!");
}