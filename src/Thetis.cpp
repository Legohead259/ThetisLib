#include "Thetis.h"

void Thetis::initialize() {
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
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GENERAL_ERROR);
        }
    }

    // Initialize SPIFFS
    if (!filesystem.begin(&SPIFFS)) {
        while (true) {
            currentState.setState(ERROR);
            errorState.setState(FILESYSTEM_MOUNT_ERROR);
        }
    }

    // Initialize system status functions
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

    // Initialize xioAPI integration
    diagLogger->info("Initializing xioAPI integration");
    if (!api.begin(&Serial)) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GENERAL_ERROR);
        }
    }
    thetisSettingsInitialize();

    // Load settings
    if (!loadConfigurationsFromJSON(true, "/config.json")) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(FILE_OPERATION_ERROR);
        }
    }

    // Initialize GPS
    if (!gps.begin()) { // Initialize GPS and check if good
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GPS_ERROR);
        }
    }
    gps.poll();
    syncInternalClockGPS(); // Attempt to sync internal clock to GPS, if it has a fix already

    // Initialize datalogger
    if (!dataLogger.begin(SD, SD_CS)) { // Initialize SD card filesystem and check if good
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(FILESYSTEM_MOUNT_ERROR);
        }
    }

    // Initialize IMU
    if (!imu.begin()) { // Check IMU initialization
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(IMU_ERROR);
        }
    }

    // Initialize magnetometer
    #ifdef MAG_ENABLE
    if (!mag.begin()) { // Check magnetometer initialization
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(IMU_ERROR);
        }
    }
    #endif // MAG_ENABLE

    // Initialize battery gauge
    #ifdef BATT_MON_ENABLE
    if (!gauge.begin()) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(BATTERY_MONITOR_ERROR);
        }
    }
    #endif // BATT_MON_ENABLED

    // Initialize WiFi
    #ifdef WIFI_ENABLE
    if (!wireless.begin()) {
        currentState.setState(ERROR);
        errorState.setState(WIFI_RADIO_ERROR);
    }
    #endif

    // Attach the log enable button interrupt
    diagLogger->info("Attaching log enable interrupt...");
    attachInterrupt(LOG_EN, logButtonISR, FALLING);
    diagLogger->info("done!");

    timerEvents.add(&gpsPollEvent);
    timerEvents.add(&gpsSyncEvent);
    timerEvents.add(&fusionUpdateEvent);
    timerEvents.add(&logWriteEvent);

    setSystemState(STANDBY);
}

void Thetis::run() {
    timerEvents.tasks();
    
    api.checkForCommand();

    // WiFi handling
    #ifdef WIFI_ENABLE
    if (settings.wirelessMode && thetisSettings.ftpEnabled) { // Only run the FTP server when the proper configs are set and the device is not logging (efficiency)
        wireless.ftpServer.handleFTP();
    }
    #endif
    
    // State updates
    updateSystemState();

    // Log Enabling handler
    static uint8_t _oldButtonPresses = 0;
    if (logButtonPresses != _oldButtonPresses && !digitalRead(LOG_EN) && millis() >= logButtonStartTime+LOG_BTN_HOLD_TIME) { // Check if BTN0 has been pressed and has been held for sufficient time
        statusChecks.isLogging = !statusChecks.isLogging;
        if (statusChecks.isLogging) {
            logWriteEvent.enable();
            dataLogger.start(SD);
            digitalWrite(SD_CS, LOW);
        }
        else {
            logWriteEvent.disable();
            dataLogger.stop();
        }
        digitalWrite(LED_BUILTIN, statusChecks.isLogging);
        _oldButtonPresses = logButtonPresses;
    }

    updateRTCms();
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

void IRAM_ATTR logButtonISR() {
    board.logButtonPresses++;
    board.logButtonStartTime = millis();
}

Thetis board(&Serial);