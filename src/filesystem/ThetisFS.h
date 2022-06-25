#define THETISFS_H

#include <FS.h>
#include <SD.h>
#include <SPI.h>

bool initSDCard(Stream &out=DEBUG_SERIAL_PORT);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels, Stream &out=DEBUG_SERIAL_PORT);
bool createDir(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool removeDir(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool readFile(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool writeFile(fs::FS &fs, const char * path, const char * message, Stream &out=DEBUG_SERIAL_PORT);
bool appendFile(fs::FS &fs, const char * path, const char * message, Stream &out=DEBUG_SERIAL_PORT);
bool renameFile(fs::FS &fs, const char * path1, const char * path2, Stream &out=DEBUG_SERIAL_PORT);
bool deleteFile(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
void testFileIO(fs::FS &fs, const char * path, Stream &out=DEBUG_SERIAL_PORT);
bool initLogFile(fs::FS &fs, char * path, Stream &out=DEBUG_SERIAL_PORT);
