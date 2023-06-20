#include "neopixel.h"
#include "../filesystem/logger.h"

// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================

Adafruit_NeoPixel pixel(1, NEOPIXEL_DATA, NEO_RGB + NEO_KHZ800);

bool initNeoPixel() {
    // TODO: Create a check that the NeoPixel gate is open
    diagLogger->info("Initializing NeoPixel...");
    digitalWrite(NEOPIXEL_EN, LOW); // Enable NeoPixel
    pixel.begin(); // Initialize pins for output
    pixel.setBrightness(50);
    pixel.show();  // Turn all LEDs off ASAP
    diagLogger->info("done!");
    return true;
}

void pulseLED(uint32_t color) {
    static uint8_t _pixelBrightness = 0; // Initialize the pixel brightness value
    static bool _isBrightnessInc = true; // Initialize the brightness increasing flag
    static long _lastCheck = 0;
    static uint8_t _i = 0;

    if (millis()-NEOPIXEL_CYCLE_TIME/(NUM_STEPS*2) >= _lastCheck) {
        // Change pixel brightness
        pixel.setBrightness(_pixelBrightness);
        pixel.setPixelColor(0, color);
        pixel.show();

        // Calculate next brightness (Linear)
        // _isBrightnessInc ? _pixelBrightness += BRIGHTNESS_STEP : _pixelBrightness -= BRIGHTNESS_STEP; // Increment or decrement the brightness based on brightness increasing flag

        // if (_pixelBrightness >= MAXIMUM_BRIGHTNESS || _pixelBrightness <= 0) _isBrightnessInc = !_isBrightnessInc; // If the pixel has met or exceeded the MAXIMUM_BRIGHTNESS or maximum darkness (0), reset the brightness increasing flag

        // Calculate next brightness (sinusoidal)
        float _iRad = _i * PI / 180;
        _pixelBrightness = sin(_iRad) * MAXIMUM_BRIGHTNESS;
        if (++_i + (180/NUM_STEPS) >= 180) _i = 0; // Increment _i and check if it is greater than 180; reset to 0 if it is
        _lastCheck = millis();
    }

    // DEBUG Statements
    // char _buf[20];
    // sprintf(_buf, "Color: %d", color);
    // diagLogger->debug(_buf);
    // sprintf(_buf, "Brightness: %d", _pixelBrightness);
    // diagLogger->debug(_buf);
    // sprintf(_buf, "Increasing: %d", _isBrightnessInc);
    // diagLogger->debug(_buf);
}

void rainbow() {
    static uint8_t _pixelState = 0;
    pixel.setPixelColor(0, Wheel(_pixelState));
    pixel.show();
    _pixelState++;
    if (_pixelState >= 255) _pixelState = 0; // Reset pixel state when max value is reached
}

uint32_t Wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return Adafruit_NeoPixel::Color((255 - wheelPos * 3)*brightness, 0, wheelPos * 3*brightness);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return Adafruit_NeoPixel::Color(0, wheelPos * 3 * brightness, (255 - wheelPos * 3)*brightness);
    }
    wheelPos -= 170;
    return Adafruit_NeoPixel::Color(wheelPos * 3 * brightness, (255 - wheelPos * 3)*brightness, 0);
}

void blinkCode(ErrorCode_t code, uint32_t color) {
    bool dash = true;
    for (int n=0; n<4; n++) {
        if (bitRead(code, n)) {
            if (dash) {
                pixel.setPixelColor(0, color); pixel.show();
                delay(DASH_ON);
                pixel.setPixelColor(0, OFF); pixel.show();
                delay(BLINK_INTERVAL);
            }
            else {
                pixel.setPixelColor(0, color); pixel.show();
                delay(DOT_ON);
                pixel.setPixelColor(0, OFF); pixel.show();
                delay(BLINK_INTERVAL);
            }
        }
        else {
            if (dash) delay(DASH_ON+BLINK_INTERVAL);
            else delay(DOT_ON+BLINK_INTERVAL);
        }
        dash = !dash;
    }
    delay(MESSAGE_INTERVAL);
}

void blink(uint8_t duration, uint32_t color) {
    pixel.setPixelColor(0, color); pixel.show();
    delay(duration);
    pixel.setPixelColor(0, OFF); pixel.show();
    delay(duration);
}

void setLEDColor(uint32_t color) {
    pixel.setPixelColor(0, color);
    pixel.show();
}