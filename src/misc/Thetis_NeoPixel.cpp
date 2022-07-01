#include "Thetis_NeoPixel.h"

// ==========================
// === NEOPIXEL FUNCTIONS ===
// ==========================


Status_t currentState = STANDBY;
Adafruit_NeoPixel pixel(1, NEOPIXEL_DATA, NEO_RGB + NEO_KHZ800);

void initNeoPixel(Adafruit_NeoPixel &strip, Stream &out) {
    out.print("Initializing NeoPixel...");
    digitalWrite(NEOPIXEL_EN, LOW); // Enable NeoPixel
    strip.begin(); // Initialize pins for output
    strip.setBrightness(50);
    strip.show();  // Turn all LEDs off ASAP
    out.println("done!");
}

void pulseLED(uint32_t color, Adafruit_NeoPixel &strip) {
    static uint8_t _pixelBrightness = 0; // Initialize the pixel brightness value
    static bool _isBrightnessInc = true; // Initialize the brightness increasing flag
    strip.setBrightness(_pixelBrightness);
    strip.setPixelColor(0, color);
    strip.show();
    _isBrightnessInc ? _pixelBrightness += BRIGHTNESS_STEP : _pixelBrightness -= BRIGHTNESS_STEP; // Increment or decrement the brightness based on brightness increasing flag
    if (_pixelBrightness >= MAXIMUM_BRIGHTNESS || _pixelBrightness <= 0) _isBrightnessInc = !_isBrightnessInc; // If the pixel has met or exceeded the MAXIMUM_BRIGHTNESS or maximum darkness (0), reset the brightness increasing flag 
}

void rainbow(Adafruit_NeoPixel &strip) {
    static uint8_t _pixelState = 0;
    strip.setPixelColor(0, Wheel(_pixelState));
    strip.show();
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

void blinkCode(ErrorCode_t code, uint32_t color, Adafruit_NeoPixel &strip) {
    bool dash = true;
    for (int n=0; n<4; n++) {
        if (bitRead(code, n)) {
            if (dash) {
                strip.setPixelColor(0, color); strip.show();
                delay(DASH_ON);
                strip.setPixelColor(0, OFF); strip.show();
                delay(BLINK_INTERVAL);
            }
            else {
                strip.setPixelColor(0, color); strip.show();
                delay(DOT_ON);
                strip.setPixelColor(0, OFF); strip.show();
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