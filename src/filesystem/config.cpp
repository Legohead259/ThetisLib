/*
 * Library for reading settings from a configuration file stored in a SD
 * Based in SDConfigFile by Bradford Needham (https://github.com/bneedhamia/sdconfigfile)
 * Licensed under LGPL version 2.1
 * a version of which should have been supplied with this file.
 */
 
#include "config.h"

Config config;

/*
 * Opens the given file on the SD card.
 * Returns true if successful, false if not.
 *
 * configFileName = the name of the configuration file on the SD card.
 *
 * NOTE: SD.begin() must be called before calling our begin().
 */
bool Config::begin(const char *configFileName, uint8_t maxLineLength) {
	_lineLength = 0;
	_lineSize = 0;
	_valueIndex = -1;
	_atEnd = true;

	/*
	* Allocate a buffer for the current line.
	*/
	_lineSize = maxLineLength + 1;
	_line = (char *) malloc(_lineSize);
  	if (_line == 0) {
		#ifdef CONFIG_DEBUG
		DEBUG_SERIAL_PORT.println("out of memory");
		#endif
		_atEnd = true;
		return false;
  	}

  /*
   * To avoid stale references to configFileName
   * we don't save it. To minimize memory use, we don't copy it.
   */
   
  	_file = SPIFFS.open(configFileName, FILE_READ);
  	if (!_file) {
		#ifdef CONFIG_DEBUG
		DEBUG_SERIAL_PORT.print("Could not open file: ");
		DEBUG_SERIAL_PORT.println(configFileName);
		#endif
		_atEnd = true;
		return false;
  	}
  
	// Initialize our reader
	_atEnd = false;
	
	return true;
}

void Config::loadConfigurations() {
	while (readNextSetting()) {
		if (nameIs("id")) {
			configData.deviceID = getIntValue();
			#ifdef CONFIG_DEBUG
			DEBUG_SERIAL_PORT.print("The ID of this device is configured to: ");
			DEBUG_SERIAL_PORT.println(configData.deviceID);
			#endif
		}
		else if (nameIs("client_ssid")) {
			strcpy(configData.ssid, getValue());
			#ifdef CONFIG_DEBUG
			DEBUG_SERIAL_PORT.print("Client SSID configured to: ");
			DEBUG_SERIAL_PORT.println(configData.ssid);
			#endif
		}
		else if (nameIs("client_password")) {
			strcpy(configData.password, getValue());
			#ifdef CONFIG_DEBUG
			DEBUG_SERIAL_PORT.print("Client password configured to: ");
			DEBUG_SERIAL_PORT.println(configData.password);
			#endif
		}
		else {
			#ifdef CONFIG_DEBUG
			DEBUG_SERIAL_PORT.print("Unknown setting name: ");
			DEBUG_SERIAL_PORT.println(getName());
			#endif
		}
	}
	end();
}

/*
 * Cleans up our SDCOnfigFile object.
 */
void Config::end() {
	if (_file) {
		_file.close();
	}

	if (_line != 0) {
		free(_line);
		_line = 0;
	}
	_atEnd = true;
}

/*
 * Reads the next name=value setting from the file.
 * Returns true if the setting was successfully read,
 * false if an error occurred or end-of-file occurred.
 */
bool Config::readNextSetting() {
	int bint;
	
	if (_atEnd) {
		return false;  // already at end of file (or error).
	}
	
	_lineLength = 0;
	_valueIndex = -1;
	
	/*
	* Assume beginning of line.
	* Skip blank and comment lines
	* until we read the first character of the key
	* or get to the end of file.
	*/
	while (true) {
		bint = _file.read();
		if (bint < 0) {
			_atEnd = true;
			return false;
		}
		
		if ((char) bint == '#') { // Check if comment line.  Read until end of line or end of file.
			while (true) {
				bint = _file.read();
				if (bint < 0) {
					_atEnd = true;
					return false;
				}
				if ((char) bint == '\r' || (char) bint == '\n') {
					break;
				}
			}
			continue; // look for the next line.
		}
		
		// Ignore line ends and blank text
		if ((char) bint == '\r' || (char) bint == '\n' || (char) bint == ' ' || (char) bint == '\t') {
			continue;
		}
			
		break; // bint contains the first character of the name
	}
	
	// Copy from this first character to the end of the line.

	while (bint >= 0 && (char) bint != '\r' && (char) bint != '\n') {
		if (_lineLength >= _lineSize - 1) { // -1 for a terminating null.
			_line[_lineLength] = '\0';
			#ifdef CONFIG_DEBUG
			DEBUG_SERIAL_PORT.print("Line too long: ");
			DEBUG_SERIAL_PORT.println(_line);
			#endif
			_atEnd = true;
			return false;
		}
		
		if ((char) bint == '=') { // Check end of Name; the next character starts the value.
			_line[_lineLength++] = '\0';
			_valueIndex = _lineLength;
		} 
		else {
			_line[_lineLength++] = (char) bint;
		}
		
		bint = _file.read();
	}
	
	if (bint < 0) {
		_atEnd = true;
		// Don't exit. This is a normal situation:
		// the last line doesn't end in newline.
	}
	_line[_lineLength] = '\0';
	
	/*
	* Sanity checks of the line:
	*   No =
	*   No name
	* It's OK to have a null value (nothing after the '=')
	*/
	if (_valueIndex < 0) {
		#ifdef CONFIG_DEBUG
		DEBUG_SERIAL_PORT.print("Missing '=' in line: ");
		DEBUG_SERIAL_PORT.println(_line);
		#endif
		_atEnd = true;
		return false;
	}
	if (_valueIndex == 1) {
		#ifdef CONFIG_DEBUG
		DEBUG_SERIAL_PORT.print("Missing Name in line: =");
		DEBUG_SERIAL_PORT.println(_line[_valueIndex]);
		#endif
		_atEnd = true;
		return false;
	}
	
	// Name starts at _line[0]; Value starts at _line[_valueIndex].
	return true;
}

/*
 * Returns true if the most-recently-read setting name
 * matches the given name, false otherwise.
 */
bool Config::nameIs(const char *name) {
  	if (strcmp(name, _line) == 0) {
    	return true;
  	}	
  	return false;
}

/*
 * Returns the name part of the most-recently-read setting.
 * or null if an error occurred.
 * WARNING: calling this when an error has occurred can crash your sketch.
 */
const char *Config::getName() {
  	if (_lineLength <= 0 || _valueIndex <= 1) {
		return 0;
	}
	return &_line[0];
}

/*
 * Returns the value part of the most-recently-read setting,
 * or null if there was an error.
 * WARNING: calling this when an error has occurred can crash your sketch.
 */
const char *Config::getValue() {
	if (_lineLength <= 0 || _valueIndex <= 1) {
		return 0;
	}
	return &_line[_valueIndex];
}

/*
 * Returns a persistent, dynamically-allocated copy of the value part
 * of the most-recently-read setting, or null if a failure occurred.
 * 
 * Unlike getValue(), the return value of this function
 * persists after readNextSetting() is called or end() is called.
 */
char *Config::copyValue() {
	char *result = 0;
	int length;

	if (_lineLength <= 0 || _valueIndex <= 1) {
		return 0; // begin() wasn't called, or failed.
	}

	length = strlen(&_line[_valueIndex]);
	result = (char *) malloc(length + 1);
	if (result == 0) {
		return 0; // out of memory
	}
	
	strcpy(result, &_line[_valueIndex]);

	return result;
}

/*
 * Returns the value part of the most-recently-read setting
 * as an integer, or 0 if an error occurred.
 */
int Config::getIntValue() {
	const char *str = getValue();
	if (!str) {
		return 0;
	}
	return atoi(str);
}

// IPAddress Config::getIPAddress(){
//   IPAddress ip(0,0,0,0);
//   const char *str = getValue();
//   int len = strlen(str);
//   char ipStr[len+1];
//   strncpy(ipStr,str,len); //char * strcpy ( char * destination, const char * source ); It is necessary to make a copy
//   ipStr[len] = '\0';
//   int i=0; int tmp;
//   const char *token = strtok(ipStr, ".");
//   while (token != NULL ) {
//     tmp = atoi(token);
//     if(tmp < 0 || tmp > 255 || i > 3){
//       ip={0,0,0,0};
//       return ip; //IP does not have more than four octets and its values are smaller than 256
//     }
//     ip[i++] = (byte) tmp;
//     token = strtok(NULL, ".");
//   }
//   return ip;
// }
/*
 * Returns the value part of the most-recently-read setting
 * as a boolean.
 * The value "true" corresponds to true;
 * all other values correspond to false.
 */
bool Config::getBooleanValue() {
	if (strcmp("true", getValue()) == 0) {
		return true;
	}
	return false;
}