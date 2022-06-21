#include <ThetisLib.h>


// =========================
// === GENERAL FUNCTIONS ===
// =========================


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
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
sensors_vec_t accelAngle;
sensors_vec_t ufAngle;
sensors_vec_t cfAngle;
sensors_vec_t kfAngle;

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
        accelSampleFreq = setSampleFrequency(dataRate);
        gyroSampleFreq = setSampleFrequency(dataRate);
        out.println("done!");
        return true;
    }
}

void pollLSM6DSO32( telemetry_t &data, Adafruit_LSM6DSO32 &imu, Stream &out) {
    DSO32_IMU.getEvent(&accel, &gyro, &temp);

    // Convert gyro to deg/s from rad/s
    gyro.gyro.x *= RAD_TO_DEG;
    gyro.gyro.y *= RAD_TO_DEG;
    gyro.gyro.z *= RAD_TO_DEG;

    // Update data packet
    data.accelX = accel.acceleration.x;
    data.accelY = accel.acceleration.y;
    data.accelZ = accel.acceleration.z;
    data.gyroX = gyro.gyro.x;
    data.gyroY = gyro.gyro.y;
    data.gyroZ = gyro.gyro.z;
    data.imuTemp = temp.temperature;

    // Debug print statements
    // Serial.printf("Accel X: %0.3f \tY: %0.3f \tZ: %0.3f m/s/s\n\r", data.accelX, data.accelY, data.accelZ);
    // Serial.printf(" Gyro X: %0.3f \tY: %0.3f \tZ: %0.3f rad/s\n\r", data.gyroX, data.gyroY, data.gyroZ);
    // Serial.printf("Temperature: %0.3f °C\n\n\r", data.imuTemp);

    // Serial plotter print statements
    // Serial.printf("%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\n\r",data.accelX, data.accelY, data.accelZ, 
    //                                                         data.linAccelX, data.linAccelY, data.linAccelZ);   
}

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

double setSampleFrequency(lsm6ds_data_rate_t dataRate) {
    double _res;
    switch (dataRate) {
        case LSM6DS_RATE_SHUTDOWN:
            _res = 0;
            break;
        case LSM6DS_RATE_12_5_HZ:
            _res = 12.5;
            break;
        case LSM6DS_RATE_26_HZ:
            _res = 26;
            break;
        case LSM6DS_RATE_52_HZ:
            _res = 52;
            break;
        case LSM6DS_RATE_104_HZ:
            _res = 104;
            break;
        case LSM6DS_RATE_208_HZ:
            _res = 208;
            break;
        case LSM6DS_RATE_416_HZ:
            _res = 416;
            break;
        case LSM6DS_RATE_833_HZ:
            _res = 833;
            break;
        case LSM6DS_RATE_1_66K_HZ:
            _res = 1660;
            break;
        case LSM6DS_RATE_3_33K_HZ:
            _res = 3330;
            break;
        case LSM6DS_RATE_6_66K_HZ:
            _res = 6660;
            break;
    }
    return _res;
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