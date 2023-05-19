//---------------------------------------------------------------------------------------------
//
// Library for logging to Serial port and SD card
//
// Copyright 2016-2018, M2M Solutions AB
// Written by Jonny Bergdahl, 2018-06-12
// Modified by Braidan Duffy, 2022-23-11
//
// Licensed under the MIT license
//
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LOGGER_H
#define LOGGER_H

#include "Arduino.h"
#include <stdarg.h>
#include <FS.h>
#include "../data.h"
#include "../misc/rtc.h"
#include <TimeLib.h>
#include <SD.h>

#define SD_FLUSH_TIMEOUT 4096
#define LOG_BTN_HOLD_TIME 500 // ms

enum class LogLevel: uint8_t {
    FATAL = 1,      // Indicates a fatal event
    ERROR,          // Indicates a major error, but not fatal
    WARN,           // Indicates a substantial event that is not an error or fatal
    INFO,           // Indicates basic information for logging
    DEBUG,          // Indicates some parameter that is less important than basic information
    VERBOSE,        // Indicates some information that is less important than debug information
    TRACE           // Information that can be used to trace down a specific bug or code execution sequence
};

class Logger {
public:
    bool begin(Print* logger, LogLevel logLevel);
    bool begin(USBCDC *logPort, LogLevel logLevel);
    bool begin(fs::SDFS &fs, uint8_t cs, LogLevel logLevel);
    bool begin(fs::SDFS &fs, uint8_t cs);
	void end();

    void writeTelemetryData();
    void start(fs::SDFS &fs);
    void stop();

    void setSerialLogger(Print *logPort);
	void setFileLogger(File* logger);
    void setLogLevel(LogLevel logLevel);
	void setLogLevel(uint8_t loglevel);
	LogLevel getLogLevel();
	LogLevel getLogLevel(uint8_t loglevel);
	void setIncludeTimestamp(bool value);
	void setIncludeLogLevel(bool value);
	void setFlushTimeout(unsigned value);

	template <class T, typename... Args>
	void fatal(T message, Args... args) {
		log(LogLevel::FATAL, false, true, message, args...);
	}
    template <class T, typename... Args>
	void error(T message, Args... args) {
		log(LogLevel::ERROR, false, true, message, args...);
	}
	template <class T, typename... Args>
	void warn(T message, Args... args) {
		log(LogLevel::WARN, false, true, message, args...);
	}
	template <class T, typename... Args>
	void info(T message, Args... args) {
		log(LogLevel::INFO, false, true, message, args...);
	}
	template <class T, typename... Args>
	void debug(T message, Args... args) {
		log(LogLevel::DEBUG, false, true, message, args...);
	}
    template <class T, typename... Args>
	void verbose(T message, Args... args) {
		log(LogLevel::VERBOSE, false, true, message, args...);
	}
	template <class T, typename... Args>
	void trace(T message, Args... args) {
		log(LogLevel::TRACE, false, true, message, args...);
	}

	template <class T, typename... Args>
	void traceStart(T message, Args... args) {
		log(LogLevel::TRACE, false, false, message, args...);
	}
	template <class T, typename... Args>
	void tracePart(T message, Args... args) {
		log(LogLevel::TRACE, true, false, message, args...);
	}
	void tracePartHexDump(const void* buffer, uint32_t size);
	void tracePartAsciiDump(const void* buffer, uint32_t size);
	template <class T, typename... Args>
	void traceEnd(T message, Args... args) {
		log(LogLevel::TRACE, true, true, message, args...);
	}

	void log(LogLevel logLevel, bool isPart, bool writeLinefeed, const char* message, ...);
	void log(LogLevel logLevel, bool isPart, bool writeLinefeed, const __FlashStringHelper* message, ...);
	void print(const char *logLine);

private:
	LogLevel _logLevel = LogLevel::INFO;
	Print* _serialLog = nullptr;
	File* _fileLog = nullptr;
	uint32_t _lastFlush = 0;
	bool _includeTimestamp = true;
	bool _includeLogLevel = true;
	bool _isActive = false;
    bool _isLogFileCreated = false;
	unsigned _flushTimeout = SD_FLUSH_TIMEOUT;
    char diagnosticLogFilename[32];
    char dataLogFilename[32];
    File dataLogFile;
    File diagLogFile;

    bool initDataLogFile(fs::SDFS &fs, char* filename);
    void getLogLevelStr(char* outStr, LogLevel loglevel);
	void printPrefix(LogLevel logLevel);
};

extern Logger dataLogger;
extern Logger diagFileLogger;
extern Logger diagPrintLogger;
extern Logger* diagLogger;

#endif // LOGGER_H