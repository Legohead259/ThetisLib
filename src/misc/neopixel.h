#ifndef THETIS_NEOPIXEL_H
#define THETIS_NEOPIXEL_H

#include <Adafruit_NeoPixel.h>
#include "../filesystem/logger.h"
#include "../subsystem.h"

#define BLINK_INTERVAL 250 // ms
#define MESSAGE_INTERVAL 1000 // ms
#define MAXIMUM_BRIGHTNESS 32
#define NUM_STEPS 50
#define BRIGHTNESS_STEP MAXIMUM_BRIGHTNESS/NUM_STEPS
#define NEOPIXEL_CYCLE_TIME 1500 // ms - Bright to bright time

const uint32_t OFF      = Adafruit_NeoPixel::Color(0, 0, 0);       // RGB
const uint32_t WHITE    = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BLUE     = Adafruit_NeoPixel::Color(0, 0, 255);
const uint32_t RED      = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t GREEN    = Adafruit_NeoPixel::Color(0, 255, 0);
const uint32_t PURPLE   = Adafruit_NeoPixel::Color(255, 0, 255);
const uint32_t AMBER    = Adafruit_NeoPixel::Color(255, 191, 0);
const uint32_t CYAN     = Adafruit_NeoPixel::Color(255, 0, 255);
const uint32_t LIME     = Adafruit_NeoPixel::Color(125, 0, 255);

const float brightness = 0.1;

enum PixelStates {
    LED_OFF,
    LED_STATIC,
    LED_PULSING,
    LED_BLINKING
};

class ThetisPixel : public Adafruit_NeoPixel, public ThetisSubsystem {
public:
    // Constructor: number of LEDs, pin number, LED type. Note: Rely on superclass constructor
    ThetisPixel(uint16_t n=1, int16_t dataPin=NEOPIXEL_DATA, uint8_t enPin=NEOPIXEL_EN, neoPixelType type=NEO_GRB+NEO_KHZ800) : 
        Adafruit_NeoPixel(n, dataPin, type) { neopixelEnPin = enPin; };
    
    bool begin() override;
    void poll() override {}
    void updateSettings() override {}
    void test() override {}

    void pulseCallback(uint32_t c);
    void rainbow();
    uint32_t Wheel(byte wheelPos);
    void blinkCode(uint8_t red, uint8_t yellow);
    void blink(unsigned long duration=BLINK_INTERVAL);
    void blinkCallback();
    void staticBlinkCallback();

    void setColor(uint32_t c) {
        setPixelColor(0, c);
        show();
    }

    void on() {
        _on = true;
        setBrightness(MAXIMUM_BRIGHTNESS);
    }

    void off() {
        _on = false;
        setBrightness(0);
    }

private:
    uint8_t neopixelEnPin;
    bool _on = true;
};

extern ThetisPixel pixel;

#endif // ndef THETIS_NEOPIXEL_H