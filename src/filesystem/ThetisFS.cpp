#include "ThetisFS.h"
 
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
    if (!fs.open(path, FILE_WRITE)) return false; // If unable to open the new log file, return an error
    Serial.printf("Logging to: %s", path);
    return true;
}