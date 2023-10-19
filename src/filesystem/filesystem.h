#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "logger.h"

enum FilesystemType {
    SPIFFS_FS,
    SD_FS
};

class ThetisFS {
public:
    bool begin(fs::SPIFFSFS* fs);
    bool begin(fs::SDFS* fs);

    // Filesystem Functions
    void listDir(const char * dirname, uint8_t levels);
    bool createDir(const char * path);
    bool removeDir(const char * path);
    bool readFile(const char * path);
    bool writeFile(const char * path, const char * message);
    bool appendFile(const char * path, const char * message);
    bool renameFile(const char * path1, const char * path2);
    bool deleteFile(const char * path);
    void testFileIO(const char * path);
    bool format();
    bool eraseAll();

    // Getters and Setters
    fs::FS* getFS() { return _fs; }

    void setFS(fs::SPIFFSFS* fs) { 
        _fsType = FilesystemType::SPIFFS_FS;
        _spiffs = fs;
        _fs = fs;
    }

    void setFS(fs::SDFS* fs) {
        _fsType = FilesystemType::SD_FS;
        _sd = fs;
        _fs = fs;
    }

    void setFS(fs::FS* fs) { _fs = fs; }

private:
    fs::SPIFFSFS* _spiffs;
    fs::SDFS* _sd;
    fs::FS* _fs;
    uint8_t _fsType;
};

extern ThetisFS filesystem;
#endif // FILESYSTEM_H