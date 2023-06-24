#include "Thetis.h"

bool Thetis::initialize() {
    // Initialize I2C bus
    Wire.begin((int) SDA, (int) SCL); // Casting to int is important as just uint8_t types will invoke the "slave" begin, not the master

    // Initialize diagnostic logger(s)
    isDebugging = digitalRead(USB_DETECT); // Check if USB is plugged in
    if (isDebugging) {
        debugSerial->begin(115200);
        debugSerial->setRxBufferSize(4096);
        #ifdef SERIAL_WAIT
        while(!Serial); // Wait for serial connection
        #endif // SERIAL_WAIT
    }

    #ifdef SERIAL_LOGGER
    if (!diagPrintLogger.begin(&Serial, LogLevel::VERBOSE)) {
        while(true);
    }
    #else
    if (!diagFileLogger.begin(SD, SD_CS, LogLevel::INFO)) {
        while(true);
    }
    #endif // SERIAL_LOGGER
    diagLogger = &diagPrintLogger;

    // Initialize NeoPixel
    if (!pixel.begin()) {
        while(true);
    }

    // Initialize SPIFFS
    if (!filesystem.begin(&SPIFFS)) {
        while (true) {
            currentState.setState(ERROR);
            errorState.setState(FILESYSTEM_MOUNT_ERROR);
        }
    }

    // Initialize system status
    diagLogger->info("Setting status callbacks");
    currentState.setStateChangeCallback([this](){ systemStatusChangeCallback(); });
    diagLogger->info("done!");

    diagLogger->debug("Adding status events to task list");
    timerEvents.add(systemStatusEvent);
    loggingNoGPS_Event.index = systemStatusEvent->index;
    loggingGPS_Event.index = systemStatusEvent->index;
    readyNoGPS_Event.index = systemStatusEvent->index;
    readyGPS_Event.index = systemStatusEvent->index;
    standby_Event.index = systemStatusEvent->index;
    booting_Event.index = systemStatusEvent->index;
    // timerEvents.add(errorStatusEvent);
    diagLogger->debug("done!");

    return true;
}

void Thetis::updateSystemState() {
    // TODO: Determine better method for IMU calibration    
    if (!statusChecks.imuCalibrated && !statusChecks.gpsFix)                                currentState.setState(STANDBY);
    else if (statusChecks.imuCalibrated && !statusChecks.gpsFix && !statusChecks.isLogging) currentState.setState(READY_NO_GPS);
    else if (statusChecks.imuCalibrated && statusChecks.gpsFix  && !statusChecks.isLogging) currentState.setState(READY_GPS);
    else if (statusChecks.imuCalibrated && !statusChecks.gpsFix && statusChecks.isLogging)  currentState.setState(LOGGING_NO_GPS);
    else if (statusChecks.imuCalibrated && statusChecks.gpsFix  && statusChecks.isLogging)  currentState.setState(LOGGING_GPS);
}

void Thetis::systemStatusChangeCallback() {
    switch (currentState.getState()) {
        case LOGGING_NO_GPS:
            systemStatusEvent = &loggingNoGPS_Event;
            break;
        case LOGGING_GPS:
            systemStatusEvent = &loggingGPS_Event;
            break;
        case READY_NO_GPS:
            systemStatusEvent = &readyNoGPS_Event;
            break;
        case READY_GPS:
            systemStatusEvent = &readyGPS_Event;
            break;
        case STANDBY:
            systemStatusEvent = &standby_Event;
            break;
        case BOOTING:
            systemStatusEvent = &booting_Event;
            break;
        default:
            return;
    }
    timerEvents.update(systemStatusEvent);
}

// inline void updateSystemLED() {
//     switch (currentState) {
//         case LOGGING_NO_GPS:
//             pixel.setPixelColor(0, BLUE); pixel.show(); // Glow solid blue
//             break;
//         case LOGGING_GPS:
//             pixel.setPixelColor(0, GREEN); pixel.show(); // Glow solid green
//             break;
//         case READY_NO_GPS:
//             // pulse(BLUE); // Pulse blue
//             break;
//         case READY_GPS:
//             // pulse(GREEN); // Pulse green
//             break;
//         case STANDBY:
//             pixel.setPixelColor(0, 255, 191, 0); pixel.show(); // Glow solid amber
//             break;
//         case BOOTING:
//             // pulse(PURPLE); // Pulse purple
//             break;
//         default:
//             pixel.setPixelColor(0, RED); pixel.show(); // Turn off LED
//             break;
//     }
// }

Thetis board(&Serial);