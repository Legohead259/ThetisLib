#include "logger.h"
#include <stdio.h>

#ifndef vsnprintf_P
#define vsnprintf_P(s, f, ...) vsnprintf((s), (f), __VA_ARGS__)
#endif

Logger dataLogger;
Logger diagFileLogger;
Logger diagPrintLogger;
Logger* diagLogger = nullptr;

bool Logger::begin(Print *logPort, LogLevel logLevel) {
	_serialLog = logPort;
	_logLevel = logLevel;
	_isActive = true;
	return true;
}

bool Logger::begin(Stream *logPort, LogLevel logLevel) {
	_serialLog = logPort;
	_logLevel = logLevel;
	_isActive = true;
	return true;
}

bool Logger::begin(fs::SDFS &fs, uint8_t cs, LogLevel logLevel) {
    if (!fs.begin(cs)) {
        return false;
    }
    for (uint16_t x = 0; x<999; x++) {
        sprintf(diagnosticLogFilename, "/dlog_%03d.txt", x);
        if (!fs.exists(diagnosticLogFilename)) break; // If a new unique log file has been named, exit loop
        if (x==999) return false; // If no unique log could be created, return an error
    }
    diagLogFile = fs.open(diagnosticLogFilename, FILE_APPEND);
    if (!diagLogFile) return false; 
    setFileLogger(&diagLogFile);
    _logLevel = logLevel;
	_isActive = true;
    return true;
}

bool Logger::begin(fs::SDFS &fs, uint8_t cs) {
	diagLogger->info("Initializing data logger...");
    if (!fs.begin(cs)) {
		diagLogger->fatal("Failed to initialize filesystem!");
        return false;
    }
    for (uint16_t x = 0; x<999; x++) {
        sprintf(dataLogFilename, "/log_%03d.bin", x);
        if (!fs.exists(dataLogFilename)) break; // If a new unique log file has been named, exit loop
        if (x==999) {
			diagLogger->fatal("No unique log file could be created!");
			return false; // If no unique log could be created, return an error
		} 
    }
	diagLogger->info("Writing data to %s", dataLogFilename);
    dataLogFile = fs.open(dataLogFilename, FILE_APPEND);
    if (!dataLogFile) {
		diagLogger->fatal("Unable to open data log file!");
		return false;
	}
    setFileLogger(&dataLogFile);
	dataLogFile.close();
	_isActive = true;
	diagLogger->info("done!");
    return true;
}

bool Logger::begin(Stream* logPort, LogLevel logLevel, LogTimeHandler cbPtr) {
	setLogTimeCallback(cbPtr);
	return begin(logPort, logLevel);
}

bool Logger::begin(fs::SDFS& fs, uint8_t cs, LogLevel logLevel, LogTimeHandler cbPtr) {
	setLogTimeCallback(cbPtr);
	return begin(fs, cs, logLevel);
}

void Logger::end() {
	_isActive = false;
}

void Logger::start(fs::SDFS &fs) {
	dataLogFile = fs.open(dataLogFilename, FILE_APPEND);
}

void Logger::stop() {
	dataLogFile.close();
}

void Logger::setSerialLogger(Print *logPort) {
    _serialLog = logPort;
}

void Logger::setFileLogger(File *logger) {
	_fileLog = logger;
	_lastFlush = millis();
}

void Logger::setLogLevel(LogLevel logLevel) {
	_logLevel = logLevel;
}

void Logger::setLogLevel(uint8_t logLevel) {
	switch (logLevel) {
		case (uint8_t) LogLevel::FATAL:
			setLogLevel(LogLevel::FATAL);
			break;
		case (uint8_t) LogLevel::ERROR:
			setLogLevel(LogLevel::ERROR);
			break;
		case (uint8_t) LogLevel::WARN:
			setLogLevel(LogLevel::WARN);
			break;
		case (uint8_t) LogLevel::INFO:
			setLogLevel(LogLevel::INFO);
			break;
		case (uint8_t) LogLevel::DEBUG:
			setLogLevel(LogLevel::DEBUG);
			break;
		case (uint8_t) LogLevel::VERBOSE:
			setLogLevel(LogLevel::VERBOSE);
			break;
		case (uint8_t) LogLevel::TRACE:
			setLogLevel(LogLevel::TRACE);
			break;
		default:
			setLogLevel(LogLevel::VERBOSE);
			break;
	}
}

LogLevel Logger::getLogLevel() {
	return _logLevel;
}

LogLevel Logger::getLogLevel(uint8_t logLevel) {
	switch (logLevel) {
		case (uint8_t) LogLevel::FATAL:
			return LogLevel::FATAL;
		case (uint8_t) LogLevel::ERROR:
			return LogLevel::ERROR;
		case (uint8_t) LogLevel::WARN:
			return LogLevel::WARN;
		case (uint8_t) LogLevel::INFO:
			return LogLevel::INFO;
		case (uint8_t) LogLevel::DEBUG:
			return LogLevel::DEBUG;
		case (uint8_t) LogLevel::VERBOSE:
			return LogLevel::VERBOSE;
		case (uint8_t) LogLevel::TRACE:
			return LogLevel::TRACE;
		default:
			return LogLevel::VERBOSE;
	}
}

void Logger::setIncludeTimestamp(bool value) {
	_includeTimestamp = value;
}

void Logger::setIncludeLogLevel(bool value) {
	_includeLogLevel = value;
}

void Logger::setFlushTimeout(unsigned value) {
	_flushTimeout = value;
}

void Logger::tracePartHexDump(const void* buffer, uint32_t size) {
	char hexValue[4];
	const uint8_t* pointer = static_cast<const uint8_t*>(buffer); 
	for (uint32_t i=0; i < size; i++)
	{
		sprintf_P(hexValue, "%02x ", pointer[i]);
		log(LogLevel::TRACE, true, false, hexValue, nullptr);
	}
}

void Logger::tracePartAsciiDump(const void* buffer, uint32_t size) {
	const uint8_t* pointer = static_cast<const uint8_t*>(buffer);	
	char output[2];
	output[1] = 0;
	for (uint32_t i=0; i < size; i++)
	{
		output[0] = char(pointer[i]);

		if (pointer[i] == 0x0a)
		{
			log(LogLevel::TRACE, true, false, "<LF>", nullptr);
			continue;
		}
		if (pointer[i] == 0x0d)
		{
			log(LogLevel::TRACE, true, false, "<CR>", nullptr);
			continue;
		}
		if (pointer[i] < 0x20 || (pointer[i] > 0x7e && pointer[i] < 0xa0))
		{
			output[0] = '.';	
		}
		log(LogLevel::TRACE, true, false, output, nullptr);
	}
}

// Private
bool Logger::initDataLogFile(fs::SDFS &fs, char* filename) {
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.print("Initializing telemetry log file...");
    #endif
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(filename, "/log_%03d.bin", x);
        if (!fs.exists(filename)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(filename, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
        DEBUG_SERIAL_PORT.println("Unable to open file for writing");
        #endif
        return false; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
    DEBUG_SERIAL_PORT.printf("Logging to: %s\n\r", filename);
    #endif

    return true;
}

void Logger::log(LogLevel logLevel, bool isPart, bool writeLinefeed, const char* format, ...) {
	char buffer[128];

	if (!_isActive || _logLevel < logLevel) {
		return;
	}
	if (!isPart) {
		printPrefix(logLevel);
	}
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	print(buffer);

	if (writeLinefeed) {
		print("\r\n");
	}
}

void Logger::log(LogLevel logLevel, bool isPart, bool writeLinefeed, const __FlashStringHelper* format, ...) {
	char buffer[128];

	if (!_isActive || _logLevel < logLevel) {
		return;
	}
	if (!isPart) {
		printPrefix(logLevel);
	}

	PGM_P pointer = reinterpret_cast<PGM_P>(format);
	va_list args;
	va_start(args, format);
	vsnprintf_P(buffer, sizeof(buffer), pointer, args);
	va_end(args);

	print(buffer);

	if (writeLinefeed) {
		print("\r\n");
	}
}

void Logger::getLogLevelStr(char* outStr, LogLevel logLevel) {
    switch(logLevel) {
        case LogLevel::FATAL:
            strcpy(outStr, "FATAL");
            break;
        case LogLevel::ERROR:
            strcpy(outStr, "ERROR");
            break;
        case LogLevel::WARN:
            strcpy(outStr, "WARN");
            break;
        case LogLevel::INFO:
            strcpy(outStr, "INFO");
            break;
        case LogLevel::DEBUG:
            strcpy(outStr, "DEBUG");
            break;
		case LogLevel::VERBOSE:
			strcpy(outStr, "VERBOSE");
            break;
        case LogLevel::TRACE:
            strcpy(outStr, "TRACE");
            break;
        default:
            strcpy(outStr, "UNKNOWN");
            break;
    }
}

void Logger::printPrefix(LogLevel logLevel) {
	if (_includeTimestamp) {
		char buffer[32];
		getSystemTime(buffer);
		print("["); print(buffer); print("] ");
	}
	if (_includeLogLevel) {
		char _buf[8];
        getLogLevelStr(_buf, logLevel);
        print("["); print(_buf); print("] ");
	}
}

void Logger::print(const char *logLine) {
	if (_serialLog != nullptr) {
		_serialLog->print(logLine);
	}
	if (_fileLog != nullptr) {
		_fileLog->print(logLine);
		if (_lastFlush < millis() + _flushTimeout) {
			_fileLog->flush();
			_lastFlush = millis();
		}
	}
}