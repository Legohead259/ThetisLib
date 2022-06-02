#include <ThetisLib.h>


// ========================
// === BNO055 FUNCTIONS ===
// ========================


bool initBNO055(Adafruit_BNO055 &imu, Stream &out) {
    out.print("Initializing BNO055...");
    digitalWrite(BNO055_RESET, HIGH); // Ensure BNO Reset is HIGH
    Wire.begin(SDA, SCL); // Initialize I2C bus with correct wires
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


bool initLSM6DSO32(Adafruit_LSM6DSO32 imu, Stream &out, 
                    lsm6dso32_accel_range_t accelRange, 
                    lsm6ds_gyro_range_t gyroRange,
                    lsm6ds_data_rate_t dataRate) {
    out.print("Initializing DSO32 IMU...");
    Wire.begin(33, 34);
    if (!imu.begin_I2C(0x6B)) {
        out.println("Failed to find LSM6DSO32 chip");
        return false;
    }
    else {
        imu.setAccelRange(accelRange);    // Set acceleration range to ±8g
        imu.setGyroRange(gyroRange);      // Set gyroscope range to ±2000 deg/sec 
        imu.setAccelDataRate(dataRate);   // Set accelerometer update rate to 52 Hz
        imu.setGyroDataRate(dataRate);    // Set gyroscope update rate to 52 Hz
        out.println("done!");
        return true;
    }
}


// ==============================
// === FILE SYSTEMS FUNCTIONS ===
// ==============================


bool initFS(fs::FS &fs, Stream &out) {
    out.print("Initializing filesystem...");
    bool _success = fs.begin();
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


// ============================
// === GPS MODULE FUNCTIONS ===
// ============================

bool initGPS(HardwareSerial &GPS, Stream &out) {
    out.print("Initializing GPS..."); // DEBUG
    GPS.begin(9600); // Begin talking with GPS at default 9600 baud.
    // TODO: Automatically determine GPS initial baudrate
    if (!GPS) {
        out.println("Failed to initialize GPS"); // DEBUG
        return false;
    }
    MicroNMEA::sendSentence(GPS, "$PMTK251,38400");         // Set GPS baudrate to 38400
    GPS.begin(38400);
    MicroNMEA::sendSentence(GPS, "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); // Enable only NMEA GGA sentences
    MicroNMEA::sendSentence(GPS, "$PMTK220,100");           // Set GPS update rate to 100 ms (10 Hz)

    // TODO: implement a check for good GPS data
    out.println("done!"); // DEBUG
    return true;
}
