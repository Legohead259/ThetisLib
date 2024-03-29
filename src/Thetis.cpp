#include "Thetis.h"

FusionAhrs ahrs;

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
    if (!diagPrintLogger.begin(&Serial, LogLevel::VERBOSE, [](char* buf){ rtc.getISO8601Time_RTC(buf); })) {
        while(true);
    }
    #else
    if (!diagFileLogger.begin(SD, SD_CS, LogLevel::INFO), [](char* buf){ rtc.getISO8601Time_RTC(buf); })) {
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
    diagLogger->info("Initializing xioAPI integration...");
    if (!api.begin(&Serial, &wireless.udpServer)) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GENERAL_ERROR);
        }
    }
    thetisSettingsInitialize();
    diagLogger->info("done!");

    // Load settings
    diagLogger->info("Loading settings from configuration file...");
    if (!loadConfigurationsFromJSON(true, "/config.json")) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(FILE_OPERATION_ERROR);
        }
    }
    diagLogger->info("done!");

    // Initialize WiFi
    if (!wireless.begin()) {
        currentState.setState(ERROR);
        errorState.setState(WIFI_RADIO_ERROR);
    }

    // Initialize GPS
    if (!gps.begin()) { // Initialize GPS and check if good
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GPS_ERROR);
        }
    }
    gps.poll();

    // Initialize RTC
    if (!rtc.begin()) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(GENERAL_ERROR);
        }
    }

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

    if (!fusionInitialize()) {
        while (true) {
            currentState.setState(ERROR);
            errorState.setState(IMU_ERROR);
        }
    }

    // Initialize battery gauge
    #ifdef BATT_MON_ENABLE
    if (!gauge.begin()) {
        while(true) {
            currentState.setState(ERROR);
            errorState.setState(BATTERY_MONITOR_ERROR);
        }
    }
    #endif // BATT_MON_ENABLED

    // Attach the log enable button interrupt
    diagLogger->info("Attaching log enable interrupt...");
    attachInterrupt(LOG_EN, logButtonISR, FALLING);
    diagLogger->info("done!");

    timerEvents.add(&gpsPollEvent);
    timerEvents.add(&inertialPollEvent);
    timerEvents.add(&magneticPollEvent);
    timerEvents.add(&fusionUpdateEvent);
    timerEvents.add(&logWriteEvent);
    timerEvents.add(&strobeEvent);
    timerEvents.add(&networkDiscoveryEvent);

    setSystemState(STANDBY);
}

self_test_report_t Thetis::selfTest() {
    // TODO: Implement a self-test feature
    self_test_report_t results = {
        .batteryMonitorPass = false,
    };
    return results;
}

void Thetis::run() {
    timerEvents.tasks();
    
    api.checkForCommand();

    // WiFi handling
    if (settings.wirelessMode && thetisSettings.ftpEnabled) { // Only run the FTP server when the proper configs are set and the device is not logging (efficiency)
        wireless.ftpServer.handleFTP();
    }
    
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

void Thetis::thetisSettingsInitialize() {
    // Assign the new entries to the array
    for (int i=0; i<numNewEntries; i++) {
        settingTable[NUM_BASE_SETTINGS+1+i] = newEntries[i];
    }

    api.setCmdReadTimeCallback([]() {
        char _buf[64];
        size_t outLen = ThetisClock::getTime_RTC(_buf);
        api.send("{\"time\":\"%s\"}", _buf);
    });

    api.setCmdWriteTimeCallback([]() {
        rtc.synchronizeClock(api.getValue<const char*>());
        api.cmdReadTime();
    });

    api.setCmdResetCallback([]() { 
        api.send("{\"reset\":null}");
        ESP.restart(); 
    });

    api.setCmdShutdownCallback([]() {
        api.send("{\"shutdown\":null}");
        ESP.restart();
        // TODO: Add an auto-latching shutoff circuit to hardware
    });

    api.setCmdStrobeCallback([this]() {
        api.send("{\"strobe\":null}");
        systemStatusEvent->disable();
        strobeEvent.enable();
    });

    api.setCmdColourCallback([this]() {
        const char* colorStr = api.getValue<const char*>();
        if (colorStr == nullptr) {
            systemStatusEvent->enable();
        }
        else {
            systemStatusEvent->disable();
            uint32_t color = hexStringToUnsignedLong(colorStr);
            pixel.setColor(color);
        }
    });

    api.setCmdHeadingCallback([]() {
        FusionAhrsSetHeading(&ahrs, api.getValue<float>());
    });

    api.setCmdSerialAccessoryCallback([]() { 
        // Ignore this command for Thetis
        return; 
    });

    api.setCmdFormatCallback([]() {
        // TODO: Call filesystem `format()` function
        // TODOTODO: Create a filesystem `format()` function
    });

    api.setCmdSelfTestCallback([this]() {
        // TODO: Call self-test function
        // TODOTODO: Create a self-test initialization function
        self_test_report_t result = selfTest();
    });

    api.setCmdBootloaderCallback([]() {
        // This is not possible on the ESP32 platform
        return;
    });

    api.setCmdFactoryCallback([]() {
        // TODO: Put the device into a factory mode state for writing to READ-ONLY settings
        // TODOTODO: Make it so only some values can be written to whilst in factory mode
    });

    api.setCmdEraseCallback([]() {
        // TODO: Call filesystem (SPIFFS) `erase()` function
        // TODOTODO: Create filesystem (SPIFFS) `erase()` function
    });
}

bool Thetis::fusionInitialize() {
    diagLogger->info("Initializing fusion algorithm");
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    const FusionAhrsSettings fusionSettings = {
        .convention = (FusionConvention) settings.ahrsAxesConvention,
        .gain = settings.ahrsGain,
        .accelerationRejection = 10.0f,
        .magneticRejection = 20.0f,
        .rejectionTimeout = 5 * (unsigned) thetisSettings.fusionUpdateRate,
    };
    FusionAhrsSetSettings(&ahrs, &fusionSettings);

    diagLogger->verbose("Setting ahrs axes convention to: %d", fusionSettings.convention);
    diagLogger->verbose("Setting AHRS gain to: %0.1f", fusionSettings.gain);
    diagLogger->verbose("Setting acceleration rejection threshold to: %0.1f", fusionSettings.accelerationRejection);
    diagLogger->verbose("Setting magnetic rejection threshold to: %0.1f", fusionSettings.magneticRejection);
    diagLogger->verbose("Setting rejection timeout to: %0.1f", fusionSettings.rejectionTimeout);

    diagLogger->info("done!");
    return true;
}

void IRAM_ATTR logButtonISR() {
    board.logButtonPresses++;
    board.logButtonStartTime = millis();
}

Thetis board(&Serial);