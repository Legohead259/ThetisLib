#include "wifi.h"

const char* PARAM_INPUT_1 = "state";

AsyncWebServer server(80); // Create AsyncWebServer object on port 80
FtpServer ftpServer;

bool initWIFIAP() {
    diagLogger->info("Starting WiFi access point...");
    // if (!WiFi.softAP(getSetting<const char*>("wiFiAPSsid"), getSetting<const char*>("wiFiAPKey"))) { // Start the access point with the config SSID and password
    if (!WiFi.softAP("Thetis-003", "")) {
        diagLogger->error("Failed to start access point!");
        return false;
    }
    diagLogger->info("done!");

    IPAddress IP = WiFi.softAPIP();
    updateSetting<const char*>((unsigned long) WIFI_IP_ADDRESS, IP.toString().c_str());
    diagLogger->info("AP IP address: %s", getSetting("wiFiIPAddress")->value);

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

    // Send a GET request to <ESP_IP>/update?state=<inputMessage>
    server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
        char inputMessage[32];
        String inputParam;
        // GET input1 value on <ESP_IP>/update?state=<inputMessage>
        if (request->hasParam("state")) {
            sprintf(inputMessage, "Got log state as: %s", request->getParam("state")->value());
            diagLogger->debug(inputMessage);
            inputParam = "state";
            // isLogging = !isLogging;
            // digitalWrite(LED_BUILTIN, isLogging);
        }
        else {
            diagLogger->warn("Invalid log state update request received!");
            inputParam = "none";
        }

        // if (isLogging) {
        //     dataLogger.start(SD);
        // }
        // else {
        //     dataLogger.stop();
        // }

        request->send(200, "text/plain", "OK");
  });

    // Send a GET request to <ESP_IP>/state
    server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // request->send(200, "text/plain", String(isLogging));
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
    diagLogger->info("Connecting to SSID: %s", getSetting<const char*>("wiFiClientSsid"));
    diagLogger->info("Using password: %s", getSetting<const char*>("wiFiClientKey"));
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
        diagLogger->verbose("Client IP Address: %s", WiFi.localIP().toString());
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