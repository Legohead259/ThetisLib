#ifndef THETIS_H
#define THETIS_H

#include "ThetisLib.h"

typedef struct self_test_report_t {
    bool spiffsPass;
    bool rtcPass;
    bool imuPass;
    bool magnetometerPass;
    bool batteryMonitorPass;
    bool batteryPass;
    bool sdCardPass;
    bool wirelessPass;
} self_test_report_t;

typedef struct status_check_t {
    bool imuCalibrated = true;
    bool gpsFix = false;
    bool isLogging = false;
} status_check_t;

void IRAM_ATTR logButtonISR();

class Thetis {
public:
    Thetis(USBCDC* s) :
    loggingNoGPS_Event("Logging, No GPS Event", 500, loggingNoGPSEventCallback, true),
    loggingGPS_Event("Logging, GPS Event", 500, loggingGPSEventCallback, true),
    readyNoGPS_Event("Ready, No GPS Event", BLINK_INTERVAL, readyNoGPSEventCallback, true),
    readyGPS_Event("Ready, GPS Event", BLINK_INTERVAL, readyGPSEventCallback, true),
    standby_Event("Standby Event", BLINK_INTERVAL, standbyEventCallback, true),
    booting_Event("Booting Event", 500, bootingEventCallback, true),
    gpsPollEvent("GPS Poll Event", GPS_POLL_INTERVAL, gpsPollCallback, true),
    gpsSyncEvent("GPS Sync Event", GPS_SYNC_INTERVAL*60000, syncInternalClockGPS, true),
    fusionUpdateEvent("Fusion Update Event", 20, fusionUpdateEventCallback, true),
    logWriteEvent("Log Write Event", 20, logWriteEventCallback, true)
    { debugSerial = s; }

    void initialize();
    void run();
        
    void updateSystemState();
    
    int8_t getSystemState() { return currentState.getState(); }
    void setSystemState(int8_t s) { currentState.setState(s); }

    unsigned long logButtonPresses;
    unsigned long logButtonStartTime;

    self_test_report_t selfTest();

private:
    USBCDC* debugSerial;
    bool isDebugging;
    status_check_t statusChecks;
    SystemStatus currentState;
    SystemError errorState;

    TimerEvent* systemStatusEvent = &booting_Event;
    TimerEvent* errorStatusEvent;
    TimerEvent loggingNoGPS_Event;
    TimerEvent loggingGPS_Event;
    TimerEvent readyNoGPS_Event;
    TimerEvent readyGPS_Event;
    TimerEvent standby_Event;
    TimerEvent booting_Event;
    TimerEvent gpsPollEvent;
    TimerEvent gpsSyncEvent;
    TimerEvent fusionUpdateEvent;
    TimerEvent logWriteEvent;

    ThetisIMU imu;
    ThetisMag mag;

    void systemStatusChangeCallback();
    void errorStatusChangeCallback();

    static void loggingNoGPSEventCallback() { 
        pixel.on();
        pixel.setColor(BLUE); 
    }
    static void loggingGPSEventCallback() {
        pixel.on();
        pixel.setColor(GREEN); 
    }
    static void readyNoGPSEventCallback() {
        pixel.setColor(BLUE); 
        pixel.blinkCallback();
    }
    static void readyGPSEventCallback() {
        pixel.setColor(GREEN); 
        pixel.blinkCallback();
    }
    static void standbyEventCallback() {
        pixel.setColor(AMBER); 
        pixel.blinkCallback();
    }
    static void bootingEventCallback() {
        pixel.on();
        pixel.setColor(PURPLE); 
    }

    static void fusionUpdateEventCallback() {
        // unsigned long _fusionStartTime = micros();
        // pollDSO32();
        // mag.poll();
        // #if defined(REV_F5) || defined(REV_G2)
        // updateVoltage();
        // #endif // defined(REV_F5) || defined(REV_G2)
        // api.sendInertial({data.accelX, data.accelY, data.accelZ, data.gyroX, data.gyroY, data.gyroZ, micros()});
        // diagLogger->trace("Time to process sensor fusion: %d ms", millis() - _fusionStartTime);
    }

    static void logWriteEventCallback() {
        // TODO: Implement log writing
        // TODO: Enable callback based on DataLoggerMessagesEnabled setting
    }

    static void gpsPollCallback() {
        gps.poll();
    }
};

extern Thetis board;
#endif // THETIS_H