#include "wifi.h"

AsyncWebServer server(80); // Create AsyncWebServer object on port 80
FtpServer ftpServer;

bool initWIFIAP() {
    diagLogger->info("Starting WiFi access point...");
    if (!WiFi.softAP(configData.ssid, "")) { // Start the access point with the config SSID and password
        diagLogger->error("Failed to start access point!");
        return false;
    }
    diagLogger->info("done!");

    IPAddress IP = WiFi.softAPIP();
    char _buf[32];
    sprintf(_buf, "AP IP address: %s", IP.toString());
    diagLogger->info(_buf);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
        diagLogger->verbose("Client requesting index!");
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
        diagLogger->verbose("Client requesting styling!");
    });

    // Start server
    server.begin();
    diagLogger->info("done!");
    return true;
}

bool initWIFIClient() {
    diagLogger->info("Starting WiFi Client...");
    // TODO: Check if SSID and password fields are available, if not return error
    if (!WiFi.begin("adafruit", "ffffffff")) {
        diagLogger->error("Failed to start WiFi client!");
        return false;
    }
    else {
        connectToWIFI(); // Attempt to connect to WiFi network
        return true;
    }
}

bool connectToWIFI() {
    char _buf[64];
    sprintf(_buf, "Connecting to SSID: %s", configData.ssid);
    diagLogger->info(_buf);
    sprintf(_buf, "Using password: %s", configData.password);
    diagLogger->info(_buf);
    unsigned long _startMillis = millis();
    while (WiFi.status() != WL_CONNECTED && millis()-_startMillis < WIFI_CONNECT_TIMEOUT*1000) { // Wait for TIMEOUT seconds while connection is tested
        delay(125);
    }

    if (WiFi.status() != WL_CONNECTED) {
        diagLogger->warn("Could not connect to WiFi network!");
        return false;
    }
    else {
        diagLogger->info("Connected!");
        sprintf(_buf, "Client IP Address: %s", WiFi.localIP().toString());
        diagLogger->verbose(_buf);
        return true;
    }
}

bool initFTPServer() {
    diagLogger->info("Starting FTP server...");
    diagLogger->verbose("Setting callback interrupt function");
    ftpServer.setCallback(_callback);
    diagLogger->verbose("Setting transfer callback interrupt function");
    ftpServer.setTransferCallback(_transferCallback);
    ftpServer.begin("braidan", "duffy");
    diagLogger->info("done!");
    return true;
}


// =====================
// === FTP FUNCTIONS ===
// =====================


void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace) {
    switch (ftpOperation) {
        case FTP_CONNECT:
            diagLogger->info("FTP client connected!");
            break;
        case FTP_DISCONNECT:
            diagLogger->warn(F("FTP client disconnected!"));
            break;
        case FTP_FREE_SPACE_CHANGE:
            char _buf[64];
            sprintf(_buf, "FTP Free space change, free %u of %u!\n", freeSpace, totalSpace);
            diagLogger->verbose(_buf);
            break;
        default:
            break;
    }
}

void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize){
    switch (ftpOperation) {
        case FTP_UPLOAD_START:
            diagLogger->info("FTP upload start!");
            break;
        case FTP_UPLOAD:
            char _buf[64];
            sprintf(_buf, "FTP upload of file %s byte %u\n", name, transferredSize);
            diagLogger->info(_buf);
            break;
        case FTP_TRANSFER_STOP:
            diagLogger->info("FTP: Finish transfer!");
            break;
        case FTP_TRANSFER_ERROR:
            diagLogger->error("FTP: Transfer error!");
            break;
        default:
            break;
    }

    /* FTP_UPLOAD_START = 0,
    * FTP_UPLOAD = 1,
    *
    * FTP_DOWNLOAD_START = 2,
    * FTP_DOWNLOAD = 3,
    *
    * FTP_TRANSFER_STOP = 4,
    * FTP_DOWNLOAD_STOP = 4,
    * FTP_UPLOAD_STOP = 4,
    *
    * FTP_TRANSFER_ERROR = 5,
    * FTP_DOWNLOAD_ERROR = 5,
    * FTP_UPLOAD_ERROR = 5
    */
}

String processor(const String &var) {
    DEBUG_SERIAL_PORT.print(var); DEBUG_SERIAL_PORT.print(": ");
    if (var == "DEVICE_ID") {
        char _deviceIDStr[4];
        sprintf(_deviceIDStr, "%03u", configData.DEVICE_ID);
        DEBUG_SERIAL_PORT.println(_deviceIDStr);
        return _deviceIDStr;
    }
    else if (var == "FW_VERSION") {
        DEBUG_SERIAL_PORT.println(STR(__FIRMWARE_VERSION__));
        return STR(__FIRMWARE_VERSION__);
    }
    else if (var == "HW_REVISION") {
        DEBUG_SERIAL_PORT.println(STR(__HARDWARE_VERSION__));
        return STR(__HARDWARE_VERSION__);
    }
    DEBUG_SERIAL_PORT.println("Unknown handle");
    return var;
}