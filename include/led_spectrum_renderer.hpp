#pragma once
#include <FastLED.h>
#include "display_config.hpp"

#define LED_PIN 4
#define SATURATION 255

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = true;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

const uint8_t kMatrixWidth = 32;
const uint8_t kMatrixHeight = 8;

extern CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
extern CRGB *const leds;

uint16_t XY(uint8_t x, uint8_t y);

uint16_t XYsafe(uint8_t x, uint8_t y);

using Spectrum = std::array<uint8_t, kMatrixWidth>;

class LEDSpectrumRenderer
{
public:
    LEDSpectrumRenderer();
    void setupLeds();
    void render(const Spectrum &newSpectrum, const DisplayConfig &config);

private:
};