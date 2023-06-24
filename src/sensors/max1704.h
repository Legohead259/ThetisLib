#include <Adafruit_MAX1704X.h>
#include "../filesystem/logger.h"
#include "../data.h"

extern Adafruit_MAX17048 battGauge;

inline bool initMAX17048() {
    diagLogger->info("Initializing MAX17048 monitor...");
    if (!battGauge.begin()) {
        diagLogger->fatal("Failed to start MAX17048 monitor!");
        return false;
    }
    else {
        diagLogger->info("done!");
        return true;
    }
}

inline void updateVoltage() {
    data.voltage = battGauge.cellVoltage();
    diagLogger->trace("Battery Voltage: %f V", data.voltage);
}