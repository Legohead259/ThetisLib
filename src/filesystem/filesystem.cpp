#include "filesystem.h"

bool Filesystem::begin(fs::SPIFFSFS* fs) {
    setFS(fs);
    _logger->info("Initializing SPIFFS filesystem...");
    bool _success = fs->begin();
    _logger->info(_success ? "done!" : "Failed to initialize filesystem!");
    return _success;
}

bool Filesystem::begin(fs::SDFS* fs) {
    setFS(fs);
    _logger->info("Initializing SD filesystem...");
    bool _success = fs->begin();
    _logger->info(_success ? "done!" : "Failed to initialize filesystem!");
    return _success;
}


// ============================
// === FILESYSTEM FUNCTIONS ===
// ============================


void Filesystem::listDir(const char * dirname, uint8_t levels) {
    _logger->info("Listing directory: %s", dirname);
    File root = _fs->open(dirname);
    if (!root) {
        _logger->error("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        _logger->warn("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            _logger->log(LogLevel::BYPASS, true, true, "  DIR : %s", file.name());
            if (levels){
                listDir(file.path(), levels--);
            }
        } 
        else {
            _logger->log(LogLevel::BYPASS, true, true, "  FILE: %s  SIZE: %d", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

bool Filesystem::createDir(const char * path) {
    _logger->info("Creating directory: %s", path);
    bool _success = _fs->mkdir(path);
    _success ? _logger->info("done!") : _logger->warn("Unable to create directory");
    return _success;
}

bool Filesystem::removeDir(const char * path) {
    _logger->info("Removing Dir: %s", path);
    bool _success = _fs->rmdir(path);
    _success ? _logger->info("done!") : _logger->warn("Unable to remove directory");
    return _success;
}

bool Filesystem::readFile(const char * path) {
    _logger->info("Reading file: %s", path);

    File file = _fs->open(path);
    if (!file) {
        _logger->error("Failed to open file for reading");
        return false;
    }

    _logger->info("Read from file: ");
    while (file.available()) {
        _logger->log(LogLevel::BYPASS, true, false, "%c", file.read());
    }
    file.close();
    return true;
}

bool Filesystem::writeFile(const char * path, const char * message) {
    _logger->info("Writing file: %s", path);

    File file = _fs->open(path, FILE_WRITE);
    if (!file) {
        _logger->error("Failed to open file for writing");
        return false;
    }

    bool _success = file.print(message);
    _success ? _logger->info("done!") : _logger->warn("Failed to write message to file");
    file.close();
    return _success;
}

bool Filesystem::appendFile(const char * path, const char * message) {
    _logger->info("Appending file: %s", path);

    File file = _fs->open(path, FILE_APPEND);
    if (!file) {
        _logger->error("Failed to open file for appending");
        return false;
    }

    bool _success = file.print(message);
    _success ? _logger->info("done!") : _logger->warn("Failed to append message to file");
    file.close();
    return _success;
}

bool Filesystem::renameFile(const char * path1, const char * path2) {
    _logger->info("Renaming file %s to %s\n\r", path1, path2);
    bool _success = _fs->rename(path1, path2);
    _success ? _logger->info("done!") : _logger->warn("Failed to rename file");
    return _success;
}

bool Filesystem::deleteFile(const char * path) {
    _logger->info("Deleting file: %s", path);
    bool _success = _fs->remove(path);
    _success ? _logger->info("File deleted") : _logger->warn("Failed to delete file");
    return _success;
}

void Filesystem::testFileIO(const char * path) {
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
        _logger->info("%u bytes read for %u ms", flen, end);
        file.close();
    } 
    else {
        _logger->error("Failed to open file for reading");
        return;
    }

    file = _fs->open(path, FILE_WRITE);
    if (!file) {
        _logger->error("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i=0; i<2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    _logger->info("%u bytes written for %u ms", 2048 * 512, end);
    file.close();
    deleteFile(path); // Clear out test file
}

bool Filesystem::format() {
    if (_fsType != FilesystemType::SPIFFS_FS) return false; // Check for correct filesystem type. Only SPIFFS supported as of now
    _logger->info("Formatting internal SPIFFS storage");
    bool _success = _spiffs->format();
    _success ? _logger->info("done!") : _logger->error("Unable to format SPIFFS");
    return _success;
}

bool Filesystem::eraseAll() {
    _logger->warn("Erasing all files on %s storage", _fsType ? "SD card" : "SPIFFS");
    File root = _fs->open("/");
    if (!root) {
        _logger->error("Failed to open root directory");
        return false;
    }
    if (!root.isDirectory()) { // TODO: Determine if necessary
        _logger->error("Not a directory");
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
    _logger->info("done!");
    return true;
}

Filesystem filesystem;
