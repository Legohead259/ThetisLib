#include "max17048.h"

bool ThetisBatteryGauge::begin() {
    diagLogger->info("Initializing MAX17048 monitor...");
    if (!Adafruit_MAX17048::begin()) {
        diagLogger->fatal("Failed to start MAX17048 monitor!");
        return false;
    }
    updateSettings();
    diagLogger->info("done!");
    return true;
}

void ThetisBatteryGauge::updateSettings() {
    resetVoltage = thetisSettings.gaugeResetVoltage;
    activityThreshold = thetisSettings.gaugeActivityThreshold;
    hibernationThreshold = thetisSettings.gaugeHibernationThreshold;
    alertMinVoltage = thetisSettings.gaugeAlertMinVoltage;
    alertMaxVoltage = thetisSettings.gaugeAlertMaxVoltage;

    diagLogger->verbose("Setting MAX17048 reset voltage to : %0.1f V", resetVoltage);
    setResetVoltage(resetVoltage);
    diagLogger->verbose("Setting MAX17048 activity threshold to : %0.1f deltaV", activityThreshold);
    setActivityThreshold(activityThreshold);
    diagLogger->verbose("Setting MAX17048 hibernation threshold to : %0.1f \%/hr", hibernationThreshold);
    setHibernationThreshold(hibernationThreshold);
    diagLogger->verbose("Setting MAX17048 min voltage alert threshold to : %0.1f V", alertMinVoltage);
    diagLogger->verbose("Setting MAX17048 max voltage alert threshold to : %0.1f V", alertMaxVoltage);
    setAlertVoltages(alertMinVoltage, alertMaxVoltage);
}

ThetisBatteryGauge gauge;