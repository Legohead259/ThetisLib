#include "filesystem.h"

bool ThetisFS::begin(fs::SPIFFSFS* fs) {
    diagLogger->info("Initializing SPIFFS filesystem...");
    setFS(fs);
    bool _success = fs->begin();
    diagLogger->info(_success ? "done!" : "Failed to initialize filesystem!");
    return _success;
}

bool ThetisFS::begin(fs::SDFS* fs) {
    setFS(fs);
    diagLogger->info("Initializing SD filesystem...");
    bool _success = fs->begin(SD_CS);
    diagLogger->info(_success ? "done!" : "Failed to initialize filesystem!");
    return _success;
}


// ============================
// === FILESYSTEM FUNCTIONS ===
// ============================


void ThetisFS::listDir(const char * dirname, uint8_t levels) {
    diagLogger->info("Listing directory: %s", dirname);
    File root = _fs->open(dirname);
    if (!root) {
        diagLogger->error("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        diagLogger->warn("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            diagLogger->log(LogLevel::BYPASS, true, true, "  DIR : %s", file.name());
            if (levels){
                listDir(file.path(), levels--);
            }
        } 
        else {
            diagLogger->log(LogLevel::BYPASS, true, true, "  FILE: %s  SIZE: %d", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

bool ThetisFS::createDir(const char * path) {
    diagLogger->info("Creating directory: %s", path);
    bool _success = _fs->mkdir(path);
    _success ? diagLogger->info("done!") : diagLogger->warn("Unable to create directory");
    return _success;
}

bool ThetisFS::removeDir(const char * path) {
    diagLogger->info("Removing Dir: %s", path);
    bool _success = _fs->rmdir(path);
    _success ? diagLogger->info("done!") : diagLogger->warn("Unable to remove directory");
    return _success;
}

bool ThetisFS::readFile(const char * path) {
    diagLogger->info("Reading file: %s", path);

    File file = _fs->open(path);
    if (!file) {
        diagLogger->error("Failed to open file for reading");
        return false;
    }

    diagLogger->info("Read from file: ");
    while (file.available()) {
        diagLogger->log(LogLevel::BYPASS, true, false, "%c", file.read());
    }
    file.close();
    return true;
}

bool ThetisFS::writeFile(const char * path, const char * message) {
    diagLogger->info("Writing file: %s", path);

    File file = _fs->open(path, FILE_WRITE);
    if (!file) {
        diagLogger->error("Failed to open file for writing");
        return false;
    }

    bool _success = file.print(message);
    _success ? diagLogger->info("done!") : diagLogger->warn("Failed to write message to file");
    file.close();
    return _success;
}

bool ThetisFS::appendFile(const char * path, const char * message) {
    diagLogger->info("Appending file: %s", path);

    File file = _fs->open(path, FILE_APPEND);
    if (!file) {
        diagLogger->error("Failed to open file for appending");
        return false;
    }

    bool _success = file.print(message);
    _success ? diagLogger->info("done!") : diagLogger->warn("Failed to append message to file");
    file.close();
    return _success;
}

bool ThetisFS::renameFile(const char * path1, const char * path2) {
    diagLogger->info("Renaming file %s to %s\n\r", path1, path2);
    bool _success = _fs->rename(path1, path2);
    _success ? diagLogger->info("done!") : diagLogger->warn("Failed to rename file");
    return _success;
}

bool ThetisFS::deleteFile(const char * path) {
    diagLogger->info("Deleting file: %s", path);
    bool _success = _fs->remove(path);
    _success ? diagLogger->info("File deleted") : diagLogger->warn("Failed to delete file");
    return _success;
}

void ThetisFS::testFileIO(const char * path) {
    File file = _fs->open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        diagLogger->info("%u bytes read for %u ms", flen, end);
        file.close();
    } 
    else {
        diagLogger->error("Failed to open file for reading");
        return;
    }

    file = _fs->open(path, FILE_WRITE);
    if (!file) {
        diagLogger->error("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i=0; i<2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    diagLogger->info("%u bytes written for %u ms", 2048 * 512, end);
    file.close();
    deleteFile(path); // Clear out test file
}

bool ThetisFS::format() {
    if (_fsType != FilesystemType::SPIFFS_FS) return false; // Check for correct filesystem type. Only SPIFFS supported as of now
    diagLogger->info("Formatting internal SPIFFS storage");
    bool _success = _spiffs->format();
    _success ? diagLogger->info("done!") : diagLogger->error("Unable to format SPIFFS");
    return _success;
}

bool ThetisFS::eraseAll() {
    diagLogger->warn("Erasing all files on %s storage", _fsType ? "SD card" : "SPIFFS");
    File root = _fs->open("/");
    if (!root) {
        diagLogger->error("Failed to open root directory");
        return false;
    }
    if (!root.isDirectory()) { // TODO: Determine if necessary
        diagLogger->error("Not a directory");
        return false;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            if (!removeDir(file.path())) return false;
        }
        else {
            if (!deleteFile(file.path())) return false;
        }
        file = root.openNextFile();
    }
    diagLogger->info("done!");
    return true;
}

ThetisFS filesystem;
