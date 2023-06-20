#ifndef THETIS_WIFIH
#define THETIS_WIFIH

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
// #include <AsyncElegantOTA.h>
#include "FtpServer.h"
#include "../xioAPI/src/xioAPI_Settings.h"
#include "../filesystem/logger.h"

#define ST(A) #A
#define STR(A) ST(A)

#define WIFI_CONNECT_TIMEOUT 10 // seconds

extern AsyncWebServer server; // Create AsyncWebServer object on port 80
extern FtpServer ftpServer;

extern const char* PARAM_INPUT_1;

bool initWIFIAP();
bool initWIFIClient();
bool connectToWIFI();
bool initFTPServer();

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize);

// Prototypes
String processor(const String &var);
String outputState();
#endif // THETIS_WIFIH