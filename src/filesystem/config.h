/*
 * Library for reading settings from a configuration file stored in a SD
 * Based in SDConfigFile by Bradford Needham (https://github.com/bneedhamia/sdconfigfile)
 * Licensed under LGPL version 2.1
 * a version of which should have been supplied with this file.
 *
 * The library supports one #define:
 *   #define SDCONFIG_DEBUG 1 // to print file error messages.
 */

#ifndef ThetisConfig_h
#define ThetisConfig_h

#include <Arduino.h>
#include <SPIFFS.h>
// #include <Ethernet.h>

struct config_data_t {
	uint8_t DEVICE_ID;
	char FW_VERSION[8];
	char HW_REVISION[8];
    
    bool wifiEnable;
	char ssid[32];
	char password[32];
    uint8_t wifiMode;

    uint8_t accelRange;
    uint16_t gyroRange;
    uint8_t magRange;
    uint16_t imuDataRate;
    uint16_t magDataRate;
    uint8_t fusionUpdateRate;

    uint8_t loggingUpdateRate;
    uint8_t eventLogLevel;
};

extern config_data_t configData;

class Config {
private:
    File _file;            // the open configuration file
    bool _atEnd;        // If true, there is no more of the file to read.
    char *_line;           // the current line of the file (see _lineLength)
                           // Allocated by begin().
    uint8_t _lineSize;     // size (bytes) of _line[]
    uint8_t _lineLength;   // length (bytes) of the current line so far.
    uint8_t _valueIndex;     // position in _line[] where the value starts
                           //  (or -1 if none)
                           // (the name part is at &_line[0])
  
public:
    bool begin(const char *configFileName, uint8_t maxLineLength);
	void loadConfigurations();
    void end();
    bool readNextSetting();
    bool nameIs(const char *name);
    const char *getName();
    const char *getValue();
    int getIntValue();
    // IPAddress getIPAddress();
    bool getBooleanValue();
    char *copyValue();
};

extern Config config;
#endif
