#include <ThetisLib.h>


// =========================
// === GENERAL FUNCTIONS ===
// =========================


// ========================
// === BNO055 FUNCTIONS ===
// ========================


bool initBNO055(Adafruit_BNO055 &imu, Stream &out) {
    out.print("Initializing BNO055...");
    digitalWrite(BNO055_RESET, HIGH); // Ensure BNO Reset is HIGH
    if (!imu.begin()) {
        out.println("Failed to initialize BNO055");
        return false;
    }
    else {
        imu.setExtCrystalUse(true);
        out.println("done!");
        return true;
    }
}


// ===========================
// === LSM6DSO32 FUNCTIONS ===
// ===========================


Adafruit_LSM6DSO32 DSO32_IMU;
double accelSampleFreq; // Accelerometer sampling frequency
double gyroSampleFreq; // Gyroscope sampling frequency

bool initLSM6DSO32( Adafruit_LSM6DSO32 &imu, 
                    Stream &out, 
                    lsm6dso32_accel_range_t accelRange, 
                    lsm6ds_gyro_range_t gyroRange,
                    lsm6ds_data_rate_t dataRate) {
    out.print("Initializing DSO32 IMU...");
    if (!imu.begin_I2C(0x6B)) {
        out.println("Failed to find LSM6DSO32 chip");
        return false;
    }
    else {
        imu.setAccelRange(accelRange);
        imu.setGyroRange(gyroRange);
        imu.setAccelDataRate(dataRate);
        imu.setGyroDataRate(dataRate);
        // setSampleFrequency(dataRate, accelSampleFreq);
        // setSampleFrequency(dataRate, gyroSampleFreq);
        out.println("done!");
        return true;
    }
}

// private void setSampleFrequency(lsm6ds_data_rate_t dataRate, double *f) {
//     switch (accelDataRate) {
//         case LSM6DS_RATE_SHUTDOWN:
//             f = 0;
//             break;
//         case LSM6DS_RATE_12_5_HZ:
//             f = 12.5;
//             break;
//         case LSM6DS_RATE_26_HZ:
//             f = 26;
//             break;
//         case LSM6DS_RATE_52_HZ:
//             f = 52;
//             break;
//         case LSM6DS_RATE_104_HZ:
//             f = 104;
//             break;
//         case LSM6DS_RATE_208_HZ:
//             f = 208;
//             break;
//         case LSM6DS_RATE_416_HZ:
//             f = 416;
//             break;
//         case LSM6DS_RATE_833_HZ:
//             f = 833;
//             break;
//         case LSM6DS_RATE_1_66K_HZ:
//             f = 1660;
//             break;
//         case LSM6DS_RATE_3_33K_HZ:
//             f = 3330;
//             break;
//         case LSM6DS_RATE_6_66K_HZ:
//             f = 6660;
//             break;
//     }
// }

// void pollLSM6DSO32( telemetry_t &data,
//                     Adafruit_LSM6DSO32 &imu,
//                     Stream &out) {
//     sensors_event_t accel;
//     sensors_event_t temp;
//     imu.fillAccelEvent(accel, millis());
//     imu.fillTempEvent(temp, millis());

//     sensors_vec_t linAccel;
//     calcLinAccel(linAccel, accel.acceleration);

//     // Update data packet
//     data.accelX = accel.acceleration.x;
//     data.accelX = accel.acceleration.y;
//     data.accelX = accel.acceleration.z;
//     data.linAccelX = linAccel.x;
//     data.linAccelY = linAccel.y;
//     data.linAccelZ = linAccel.z;    
// }

// void calcLinAccel(sensors_vec_t &linAccel, sensors_vec_t &accel, uint8_t n, double fc, double fs) {
//     double fn = 2 * fc / fs; // Normalized cut-off frequency
//     auto filter = butterworth<n>(fn); // Create a Butterworth filter of nth-order with a normalized cutoff frequency
    
//     // Calculate the gravity vector from the low-pass Butterworth filter
//     sensors_vec_t gravity;
//     gravity.x = filter(accel.x);
//     gravity.y = filter(accel.y);
//     gravity.z = filter(accel.z);

//     // Calculate the linear acceleration by removing the gravity signal
//     linAccel.x = accel.x - gravity.x;
//     linAccel.y = accel.y - gravity.y;
//     linAccel.z = accel.z - gravity.z;
// }

// ==============================
// === FILE SYSTEMS FUNCTIONS ===
// ==============================


bool initSDCard(Stream &out) {
    out.print("Initializing filesystem...");
    bool _success = SD.begin();
    _success ? out.println("done!") : out.println("Failed to initialized filesystem");
    return _success;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels, Stream &out) {
    out.printf("Listing directory: %s\n\r", dirname);

    File root = fs.open(dirname);
    if(!root){
        out.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        out.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            out.print("  DIR : ");
            out.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels--, out);
            }
        } else {
            out.print("  FILE: ");
            out.print(file.name());
            out.print("  SIZE: ");
            out.println(file.size());
        }
        file = root.openNextFile();
    }
}

bool createDir(fs::FS &fs, const char * path, Stream &out) {
    out.printf("Creating Dir: %s\n\r", path);
    bool _success = fs.mkdir(path);
    _success ? out.println("Dir created") : out.println("mkdir failed");
    return _success;
}

bool removeDir(fs::FS &fs, const char * path, Stream &out) {
    out.printf("Removing Dir: %s\n\r", path);
    bool _success = fs.rmdir(path);
    _success ? out.println("Dir removed") : out.println("rmdir failed");
    return _success;
}

bool readFile(fs::FS &fs, const char * path, Stream &out) {
    out.printf("Reading file: %s\r", path);

    File file = fs.open(path);
    if(!file){
        out.println("Failed to open file for reading");
        return false;
    }

    out.print("Read from file: ");
    while(file.available()){
        out.write(file.read());
    }
    file.close();
    return true;
}

bool writeFile(fs::FS &fs, const char * path, const char * message, Stream &out) {
    out.printf("Writing file: %s\n\r", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        out.println("Failed to open file for writing");
        return false;
    }

    bool _success = file.print(message);
    _success ? out.println("Message appended!") : out.println("Failed to append to file");
    file.close();
    return _success;
}

bool appendFile(fs::FS &fs, const char * path, const char * message, Stream &out) {
    out.printf("Appending to file: %s\n\r", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        out.println("Failed to open file for appending");
        return false;
    }

    bool _success = file.print(message);
    _success ? out.println("Message appended!") : out.println("Failed to append to file");
    file.close();
    return _success;
}

bool renameFile(fs::FS &fs, const char * path1, const char * path2, Stream &out) {
    out.printf("Renaming file %s to %s\n\r", path1, path2);
    bool _success = fs.rename(path1, path2);
    _success ? out.println("Renamed file") : out.println("Failed to rename file");
    return _success;
}

bool deleteFile(fs::FS &fs, const char * path, Stream &out) {
    Serial.printf("Deleting file: %s\n\r", path);
    bool _success = fs.remove(path);
    _success ? out.println("Deleted file") : out.println("Failed to delete file");
    return _success;
}

void testFileIO(fs::FS &fs, const char * path, Stream &out) {
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        out.printf("%u bytes read for %u ms\n\r", flen, end);
        file.close();
    } else {
        out.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        out.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    out.printf("%u bytes written for %u ms\n\r", 2048 * 512, end);
    file.close();
}

bool initLogFile(fs::FS &fs, char * path, Stream &out) {
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(path, "/log_%03d.csv", x);
        if (!fs.exists(path)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(path)) return false; // If unable to open the new log file, return an error
    Serial.printf("Logging to: %s", path);
    return true;
}

bool writeTelemetryData(fs::FS &fs, const char * path, telemetry_t &data, Stream &out) {
    out.printf("Writing telemetry packet to: %s", path);
    File _dataFile = fs.open(path, FILE_APPEND);
    if (!_dataFile) {
        out.printf("Could not write to %s", path);
        return false;
    }

    _dataFile.print(data.timestamp);
    _dataFile.printf("%0.3f,", data.voltage);
    _dataFile.printf("%d,", data.GPSFix);
    _dataFile.printf("%d,", data.numSats);
    _dataFile.printf("%d,", data.HDOP);
    _dataFile.printf("%0.3f,", data.latitude / 1E6);
    _dataFile.printf("%0.3f,", data.longitude / 1E6);
    _dataFile.printf("%0.3f,", data.GPSSpeed / 1E3);
    _dataFile.printf("%0.3f,", data.GPSCourse / 1E3);
    _dataFile.printf("%d,", data.sysCal);
    _dataFile.printf("%d,", data.gyroCal);
    _dataFile.printf("%d,", data.accelCal);
    _dataFile.printf("%d,", data.magCal);
    _dataFile.printf("%0.3f,", data.accelX);
    _dataFile.printf("%0.3f,", data.accelY);
    _dataFile.printf("%0.3f,", data.accelZ);
    _dataFile.printf("%0.3f,", data.magX);
    _dataFile.printf("%0.3f,", data.magY);
    _dataFile.printf("%0.3f,", data.magZ);
    _dataFile.printf("%0.3f,", data.gyroX);
    _dataFile.printf("%0.3f,", data.gyroY);
    _dataFile.printf("%0.3f,", data.gyroZ);
    _dataFile.printf("%0.3f,", data.roll);
    _dataFile.printf("%0.3f,", data.pitch);
    _dataFile.printf("%0.3f,", data.yaw);
    _dataFile.printf("%0.3f,", data.linAccelX);
    _dataFile.printf("%0.3f,", data.linAccelY);
    _dataFile.printf("%0.3f,", data.linAccelZ);
    _dataFile.printf("%0.3f,", data.quatW);
    _dataFile.printf("%0.3f,", data.quatX);
    _dataFile.printf("%0.3f,", data.quatY);
    _dataFile.printf("%0.3f,", data.quatZ);
    _dataFile.printf("%0.3f,", data.imuTemp);
    _dataFile.printf("%d,", data.state);
    _dataFile.print(data.packetSize);
    _dataFile.println();
    _dataFile.close();

    out.printf("Wrote to: %s\n\r", path);
    return true;
}


// ============================
// === GPS MODULE FUNCTIONS ===
// ============================


HardwareSerial& GPS = GPS_SERIAL_PORT;
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
bool ledState = LOW;
volatile bool ppsTriggered = false;

bool initGPS(HardwareSerial &gps, Stream &out) {
    out.print("Initializing GPS..."); // DEBUG
    gps.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!gps) {
        out.println("Failed to initialize GPS"); // DEBUG
        return false;
    }
    delay(10);
    MicroNMEA::sendSentence(gps, "$PMTK251,38400");         // Set GPS baudrate to 38400
    delay(10);
    gps.begin(38400);
    MicroNMEA::sendSentence(gps, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable NMEA RMC and GGA sentences
    MicroNMEA::sendSentence(gps, "$PMTK220,100");           // Set GPS update rate to 100 ms (10 Hz)

    // TODO: implement a check for good GPS data
    out.println("done!"); // DEBUG
    return true;
}

void ppsHandler(void) {
    ppsTriggered = true;
}

void printUnknownSentence(MicroNMEA &nmea, Stream &out) {
    out.println();
	out.print("Unknown sentence: ");
	out.println(nmea.getSentence());
}


// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================


Status_t currentState = STANDBY;
Adafruit_NeoPixel pixel(1, NEOPIXEL_DATA, NEO_RGB + NEO_KHZ800);

void initNeoPixel(Adafruit_NeoPixel &strip, Stream &out) {
    out.print("Initializing NeoPixel...");
    digitalWrite(NEOPIXEL_EN, LOW); // Enable NeoPixel
    strip.begin(); // Initialize pins for output
    strip.setBrightness(50);
    strip.show();  // Turn all LEDs off ASAP
    out.println("done!");
}

void pulseLED(uint32_t color, Adafruit_NeoPixel &strip) {
    static uint8_t _pixelBrightness = 0; // Initialize the pixel brightness value
    static bool _isBrightnessInc = true; // Initialize the brightness increasing flag
    strip.setBrightness(_pixelBrightness);
    strip.setPixelColor(0, color);
    strip.show();
    _isBrightnessInc ? _pixelBrightness += BRIGHTNESS_STEP : _pixelBrightness -= BRIGHTNESS_STEP; // Increment or decrement the brightness based on brightness increasing flag
    if (_pixelBrightness >= MAXIMUM_BRIGHTNESS || _pixelBrightness <= 0) _isBrightnessInc = !_isBrightnessInc; // If the pixel has met or exceeded the MAXIMUM_BRIGHTNESS or maximum darkness (0), reset the brightness increasing flag 
}

void rainbow(Adafruit_NeoPixel &strip) {
    static uint8_t _pixelState = 0;
    strip.setPixelColor(0, Wheel(_pixelState));
    strip.show();
    _pixelState++;
    if (_pixelState >= 255) _pixelState = 0; // Reset pixel state when max value is reached
}

uint32_t Wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return Adafruit_NeoPixel::Color((255 - wheelPos * 3)*brightness, 0, wheelPos * 3*brightness);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return Adafruit_NeoPixel::Color(0, wheelPos * 3 * brightness, (255 - wheelPos * 3)*brightness);
    }
    wheelPos -= 170;
    return Adafruit_NeoPixel::Color(wheelPos * 3 * brightness, (255 - wheelPos * 3)*brightness, 0);
}

void blinkCode(ErrorCode_t code, uint32_t color, Adafruit_NeoPixel &strip) {
    bool dash = true;
    for (int n=0; n<4; n++) {
        if (bitRead(code, n)) {
            if (dash) {
                strip.setPixelColor(0, color); strip.show();
                delay(DASH_ON);
                strip.setPixelColor(0, OFF); strip.show();
                delay(BLINK_INTERVAL);
            }
            else {
                strip.setPixelColor(0, color); strip.show();
                delay(DOT_ON);
                strip.setPixelColor(0, OFF); strip.show();
                delay(BLINK_INTERVAL);
            }
        }
        else {
            if (dash) delay(DASH_ON+BLINK_INTERVAL);
            else delay(DOT_ON+BLINK_INTERVAL);
        }
        dash = !dash;
    }
    delay(MESSAGE_INTERVAL);
}


// =========================
// === LOGGING FUNCTIONS ===
// =========================


long logButtonStartTime = 0;
bool logButtonPressed = false;
bool isLogging = false;

void IRAM_ATTR  logButtonISR() {
    if (digitalRead(LOG_EN)) {             // Button is Pressed
        logButtonPressed = true;            // Set the log button pressed flag
        logButtonStartTime = millis();      // Start the log button hold timer
        // Serial.println("Button pressed!");    // DEBUG
        // Serial.printf("logButtonPressed: %s\n", logButtonPressed ? "true" : "false"); // DEBUG
        // Serial.printf("logButtonStartTime: %d\n\n", logButtonStartTime); // DEBUG
    }
    else {
        logButtonPressed = false;           // reset the log button pressed flag
        // Serial.println("Button released!");   // DEBUG
        // Serial.printf("logButtonPressed: %s\n\n", logButtonPressed ? "true" : "false"); // DEBUG
    }
}