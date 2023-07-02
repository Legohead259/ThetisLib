#include <Adafruit_MAX1704X.h>
#include "../filesystem/logger.h"
#include "../settings.h"
#include "../subsystem.h"

class ThetisBatteryGauge : public Adafruit_MAX17048, ThetisSubsystem {
public:
    bool begin() override;
    void poll() override { return; }
    void updateSettings() override;
    void test() override {}

private:
    float resetVoltage;
    float activityThreshold;
    float hibernationThreshold;
    float alertMinVoltage;
    float alertMaxVoltage;   
};

extern ThetisBatteryGauge gauge;