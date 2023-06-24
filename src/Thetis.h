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

class Thetis {
public:
    Thetis(USBCDC* s) :
    loggingNoGPS_Event("Logging, No GPS Event", 500, loggingNoGPSEventCallback, true),
    loggingGPS_Event("Logging, GPS Event", 500, loggingGPSEventCallback, true),
    readyNoGPS_Event("Ready, No GPS Event", BLINK_INTERVAL, readyNoGPSEventCallback, true),
    readyGPS_Event("Ready, GPS Event", BLINK_INTERVAL, readyGPSEventCallback, true),
    standby_Event("Standby Event", BLINK_INTERVAL, standbyEventCallback, true),
    booting_Event("Booting Event", 500, bootingEventCallback, true) 
    { debugSerial = s; }

    bool initialize();
        
    void updateSystemState();
    
    int8_t getSystemState() { return currentState.getState(); }
    void setSystemState(int8_t s) { currentState.setState(s); }

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
};

extern Thetis board;
#endif // THETIS_H