#include "wifi.h"

bool ThetisWiFi::begin() {
    diagLogger->info("Starting WiFi service");
    updateSettings();
    bool _success = false;

    switch(settings.wirelessMode) {
        case WIRELESS_DISABLED:
            return true;
        case WIRELESS_AP:
            _success = beginWiFiAP();
            break;
        case WIRELESS_CLIENT:
            _success = beginWiFiClient();
            break;
        default:
            return false;
    }

    udpServer.begin(udpIPAddress, getSetting<uint16_t>("udpReceivePort"));

    // // Route for root / web page
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //     request->send(SPIFFS, "/index.html", String(), false, processor);
    //     diagLogger->verbose("Client requesting index!");
    // });

    // // Route to load style.css file
    // server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    //     request->send(SPIFFS, "/style.css", "text/css");
    //     diagLogger->verbose("Client requesting styling!");
    // });

    // // Send a GET request to <ESP_IP>/update?state=<inputMessage>
    // server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //     char inputMessage[32];
    //     String inputParam;
    //     // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    //     if (request->hasParam("state")) {
    //         sprintf(inputMessage, "Got log state as: %s", request->getParam("state")->value());
    //         diagLogger->debug(inputMessage);
    //         inputParam = "state";
    //         // isLogging = !isLogging;
    //         // digitalWrite(LED_BUILTIN, isLogging);
    //     }
    //     else {
    //         diagLogger->warn("Invalid log state update request received!");
    //         inputParam = "none";
    //     }

    //     // if (isLogging) {
    //     //     dataLogger.start(SD);
    //     // }
    //     // else {
    //     //     dataLogger.stop();
    //     // }

    //     request->send(200, "text/plain", "OK");
    // });

    // // Send a GET request to <ESP_IP>/state
    // server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //     // request->send(200, "text/plain", String(isLogging));
    // });

    // // Start server
    // server.begin();

    return _success;
}

void ThetisWiFi::updateSettings() {
    udpIPAddress = udpIPAddress.fromString(getSetting<const char*>("udpIPAddress"));
    clientIPAddress = clientIPAddress.fromString(getSetting<const char*>("wiFiClientIPAddress"));
}

bool ThetisWiFi::beginWiFiAP() {
    diagLogger->info("Starting WiFi access point...");
    if (!WiFi.softAP(getSetting<const char*>("wiFiAPSsid"), getSetting<const char*>("wiFiAPKey"))) {
        diagLogger->error("Failed to start access point!");
        return false;
    }
    diagLogger->info("done!");

    ipAddress = WiFi.softAPIP();
    updateSetting<const char*>((unsigned long) WIFI_IP_ADDRESS, ipAddress.toString().c_str());
    diagLogger->info("AP IP address: %s", settings.wiFiIPAddress);

    diagLogger->info("done!");
    return true;
}

bool ThetisWiFi::beginWiFiClient() {
    diagLogger->info("Starting WiFi Client...");
    if (!connectToWiFi()) { // Attempt to connect to WiFi network
        return false;
    }
    return true;
}

bool ThetisWiFi::connectToWiFi() {
    diagLogger->info("Connecting to SSID: %s", getSetting<const char*>("wiFiClientSsid"));
    diagLogger->info("Using password: %s", getSetting<const char*>("wiFiClientKey"));
    if (!WiFi.begin(getSetting<const char*>("wiFiClientSsid"), getSetting<const char*>("wiFiClientKey"))) {
        diagLogger->error("Failed to start WiFi client!");
        return false;
    }
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
        updateSetting<const char*>((unsigned long) WIFI_CLIENT_IP_ADDRESS, WiFi.localIP().toString().c_str());
        diagLogger->info("Client IP Address: %s", WiFi.localIP().toString().c_str());
        return true;
    }
}

bool ThetisWiFi::initFTPServer() {
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


static void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace) {
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

static void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize) {
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


// ========================
// === SERVER FUNCTIONS ===
// ========================


String processor(const String& var) {
    //Serial.println(var);
    if (var == "BUTTONPLACEHOLDER") {
        String buttons ="";
        String outputStateValue = outputState();
        buttons += "<h4>Log Enable <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
        return buttons;
    }
    return String();
}

String outputState() {
    return "";
}

ThetisWiFi wireless;