#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include <WiFi.h>
#include <WiFiUdp.h>
// #include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
// #include <AsyncElegantOTA.h>
#include "FtpServer.h"
#include "../xioAPI/src/xioAPI_Settings.h"
#include "../filesystem/logger.h"
#include "../settings.h"
#include "../subsystem.h"

#define ST(A) #A
#define STR(A) ST(A)

#define WIFI_CONNECT_TIMEOUT 10 // seconds

class ThetisWiFi : ThetisSubsystem {

public:
    // ThetisWiFi() : server(80) {}

    bool begin() override;
    void updateSettings() override;
    void poll() override { return; }
    void test() override {}

    bool beginWiFiAP();
    bool beginWiFiClient();
    bool connectToWiFi();
    bool initFTPServer();
    
    // AsyncWebServer server; // Create AsyncWebServer object on port 80
    WiFiUDP udpServer;
    FtpServer ftpServer;

    IPAddress ipAddress;
    IPAddress clientIPAddress;
    IPAddress udpIPAddress;
private:
    const char* PARAM_INPUT_1 = "state";
};

// Prototypes
static void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
static void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize);
String processor(const String &var);
String outputState();

extern ThetisWiFi wireless;
#endif // THETIS_WIFIH