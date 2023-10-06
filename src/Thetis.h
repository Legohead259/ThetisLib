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

extern FusionAhrs ahrs;

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
    inertialPollEvent("Inertial Poll Event", 15, inertialPollEventCallback, true),
    magneticPollEvent("Magnetic Poll Event", 15, magneticPollEventCallback, true),
    fusionUpdateEvent("Fusion Update Event", 15, fusionUpdateEventCallback, true),
    logWriteEvent("Log Write Event", 15, logWriteEventCallback, false),
    strobeEvent("Strobe Event", BLINK_INTERVAL/2, strobeEventCallback, false, 5000, [this](){ systemStatusEvent->enable(); }),
    networkDiscoveryEvent("Network Discovery Event", 1000, networkDiscoveryEventCallback, true)
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
    TimerEvent inertialPollEvent;
    TimerEvent magneticPollEvent;
    TimerEvent fusionUpdateEvent;
    TimerEvent logWriteEvent;
    TimerEvent strobeEvent;
    TimerEvent networkDiscoveryEvent;

    FusionMatrix gyroscopeMisalignment;
    FusionVector gyroscopeSensitivity;
    FusionVector gyroscopeOffset;
    FusionMatrix accelerometerMisalignment;
    FusionVector accelerometerSensitivity;
    FusionVector accelerometerOffset;
    FusionMatrix softIronMatrix;
    FusionVector hardIronOffset;

    void systemStatusChangeCallback();
    void errorStatusChangeCallback();
    void thetisSettingsInitialize();
    bool fusionInitialize();
    void fusionUpdateSettings();

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

    static void strobeEventCallback() {
        pixel.setColor(WHITE);
        pixel.blinkCallback();
    }

    static void networkDiscoveryEventCallback() {
        NetworkAnnouncement message = {
            .sync               = 1, // TODO: Determine how to better incorporate sync words
            .tcpPort            = settings.tcpPort,
            .udpSendPort        = settings.udpSendPort,
            .udpReceivePort     = settings.udpReceivePort,
            .rssiPercentage     = 100, // TODO: Create a wireless.getRSSI() function. 0-100% or -1% for AP mode
            .batteryPercentage  = (uint8_t) gauge.cellPercent(),
            .chargingStatus     = 1, // TODO: Create gauge.getChargingStatus() function. 0 = NC, 1 = Charging, 2 = Charging complete
        };
        memcpy(message.displayName, settings.deviceName, 32);
        memcpy(message.serialNumber, settings.serialNumber, 20);
        memcpy(message.ipAddress, settings.wiFiIPAddress, 16);
        api.sendNetworkAnnouncement(message);
    }

    static void inertialPollEventCallback() {
        uint32_t timestamp = micros();
        imu.poll();

        // Send IMU data messages
        api.sendInertialMessage(InertialMessage{ 
            imu.accelerometerData.array[0], 
            imu.accelerometerData.array[1], 
            imu.accelerometerData.array[2],
            imu.gyroscopeData.array[0], 
            imu.gyroscopeData.array[1], 
            imu.gyroscopeData.array[2], 
            timestamp 
        });
    }

    static void magneticPollEventCallback() {
        uint32_t timestamp = micros();
        mag.poll();

        api.sendMagnetometerMessage(MagnetometerMessage{
            mag.magnetometerData.array[0], 
            mag.magnetometerData.array[1], 
            mag.magnetometerData.array[2], 
            timestamp
        });
    }

    static void fusionUpdateEventCallback() {
        uint32_t timestamp = micros();

        // Calculate delta time (in seconds) to account for gyroscope sample clock error
        static uint32_t previousTimestamp;
        const float deltaTime = (float) (timestamp - previousTimestamp) / (float) CLOCKS_PER_SEC;
        previousTimestamp = timestamp;

        // Update AHRS algorithm
        FusionAhrsUpdate(&ahrs, imu.gyroscopeData, imu.accelerometerData, mag.magnetometerData, deltaTime);

        // Send orientation data messages
        FusionQuaternion quat = FusionAhrsGetQuaternion(&ahrs);
        FusionEuler euler = FusionQuaternionToEuler(quat);

        api.sendQuaternionMessage(QuaternionMessage{
            quat.array[0], 
            quat.array[1], 
            quat.array[2], 
            quat.array[3], 
            timestamp
        });
        api.sendEulerMessage(EulerMessage{
            euler.array[0], 
            euler.array[1], 
            euler.array[2], 
            timestamp
        });
    }

    static void logWriteEventCallback() {
        // TODO: Enable callback based on DataLoggerMessagesEnabled setting
        // TODO: Deprecate dependency on statusChecks.isLogging flag
        // diagLogger->debug("Data buffer empty: %s", dataASCIIBuffer.isEmpty() ? "true" : "false");
        // diagLogger->debug("Data buffer full: %s", dataASCIIBuffer.isFull() ? "true" : "false");
        // diagLogger->debug("Data buffer size: %d", dataASCIIBuffer.size());
        while(!dataASCIIBuffer.isEmpty()) {
            dataLogger.dataLogFile.write(dataASCIIBuffer.shift());
        }
    }

    static void gpsPollCallback() {
        gps.poll();
        api.sendDataMessage("P,%lu,%u,%u,%u,%ld,%ld,%ld,%ld", 
                                micros(), 
                                gps.isValid(), 
                                gps.getNumSatellites(), 
                                gps.getHDOP(), 
                                gps.getLatitude(), 
                                gps.getLongitude(), 
                                gps.getSpeed(), 
                                gps.getCourse()
                            );
    }
};

extern Thetis board;
#endif // THETIS_H