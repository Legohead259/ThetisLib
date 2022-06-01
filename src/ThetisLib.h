#ifndef THETISLIB_H
#define THETISLIB_H
#define THETISLIB_VERSION "0.1.0"

#include <Arduino.h>


// ==============================
// === FILE SYSTEMS FUNCTIONS ===
// ==============================


#include "FS.h"
#include <SD.h>
#include <SPI.h>

bool initFS(fs::FS &fs, Stream &out);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels, Stream &out);
bool createDir(fs::FS &fs, const char * path, Stream &out);
bool removeDir(fs::FS &fs, const char * path, Stream &out);
bool readFile(fs::FS &fs, const char * path, Stream &out);
bool writeFile(fs::FS &fs, const char * path, const char * message, Stream &out);
bool appendFile(fs::FS &fs, const char * path, const char * message, Stream &out);
bool renameFile(fs::FS &fs, const char * path1, const char * path2, Stream &out);
bool deleteFile(fs::FS &fs, const char * path, Stream &out);
void testFileIO(fs::FS &fs, const char * path, Stream &out);

#endif