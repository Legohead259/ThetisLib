#include "neopixel.h"

//     Adafruit_NeoPixel pixel(1, NEOPIXEL_DATA, NEO_RGB + NEO_KHZ800);

bool ThetisPixel::begin() {
    // TODO: Create a check that the NeoPixel gate is open
    diagLogger->info("Initializing NeoPixel...");
    digitalWrite(neopixelEnPin, LOW); // Enable NeoPixel
    Adafruit_NeoPixel::begin(); // Initialize pins for output
    setBrightness(50);
    setPixelColor(1, OFF);
    show();  // Turn all LEDs off ASAP

    timerEvents.add(&blinkLEDEvent);
    timerEvents.add(&pulseLEDEvent);
    timerEvents.add(&rainbowLEDEvent);

    diagLogger->info("done!");
    return true;
}

//NEOPIXEL_CYCLE_TIME/(NUM_STEPS*2)

void ThetisPixel::pulse() {
    static uint8_t _pixelBrightness = 0; // Initialize the pixel brightness value
    static bool _isBrightnessInc = true; // Initialize the brightness increasing flag
    static uint8_t _i = 0;

    // Change pixel brightness
    setBrightness(_pixelBrightness);

    // Calculate next brightness (sinusoidal)
    float _iRad = _i * PI / 180;
    _pixelBrightness = sin(_iRad) * MAXIMUM_BRIGHTNESS;
    if (++_i + (180/NUM_STEPS) >= 180) _i = 0; // Increment _i and check if it is greater than 180; reset to 0 if it is
}

void ThetisPixel::rainbow() {
    static uint8_t _pixelState = 0;
    setPixelColor(0, Wheel(_pixelState));
    show();
    if (++_pixelState >= 255) _pixelState = 0; // Reset pixel state when max value is reached
}

uint32_t ThetisPixel::Wheel(byte wheelPos) {
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

void ThetisPixel::blinkCode(uint8_t red, uint8_t yellow) {
    for (uint8_t r=0; r<red; r++) { // Do the red blinks
        setPixelColor(0, RED); 
        show();
        blink();
    }
    for (uint8_t y=0; y<yellow; y++) { // Do the yellow blinks
        setPixelColor(0, AMBER); 
        show();
        blink();
    }
    delay(MESSAGE_INTERVAL);
}

void ThetisPixel::blink(unsigned long duration) {
    setBrightness(MAXIMUM_BRIGHTNESS);
    delay(duration);
    setBrightness(0);
    delay(duration);
}

void ThetisPixel::blinkCallback() {
    _on ? setBrightness(MAXIMUM_BRIGHTNESS) : setBrightness(0);
    _on = !_on;
}

void ThetisPixel::staticBlinkCallback() {
    this->blinkCallback();
}

ThetisPixel pixel(1, NEOPIXEL_DATA, NEOPIXEL_EN, NEO_GRB+NEO_KHZ800);