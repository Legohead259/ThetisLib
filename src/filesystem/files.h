#define THETISFS_H

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "logger.h"

extern char wifiSSID[20];
extern char wifi_pass[20];
extern char ftp_name[20];
extern char ftp_pass[20];

bool initSDCard();
bool initSPIFFS();

// Filesystem Functions
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
bool createDir(fs::FS &fs, const char * path);
bool removeDir(fs::FS &fs, const char * path);
bool readFile(fs::FS &fs, const char * path);
bool writeFile(fs::FS &fs, const char * path, const char * message);
bool appendFile(fs::FS &fs, const char * path, const char * message);
bool renameFile(fs::FS &fs, const char * path1, const char * path2);
bool deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
void authDecode();

