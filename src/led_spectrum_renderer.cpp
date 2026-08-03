#include "led_spectrum_renderer.h"
#include <queue>

CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds = leds_plus_safety_pixel + 1;

LEDSpectrumRenderer::LEDSpectrumRenderer()
{
}

// Initializes Red in the top corners, Blue in the bottom ones
// Intended to be able to place the LED matrix in the correct way
void topBottomColors()
{
    // Top
    leds[XYsafe(0, kMatrixHeight)] = CRGB::Red;
    leds[XYsafe(kMatrixWidth, kMatrixHeight)] = CRGB::Red;

    // Bottom
    leds[XYsafe(0, 0)] = CRGB::Green;
    leds[XYsafe(kMatrixWidth, 0)] = CRGB::Green;
}

void LEDSpectrumRenderer::setupLeds()
{
    FastLED.clear();
    Serial.printf("Setting up LEDs...\n");
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();
    topBottomColors();
    FastLED.show();
    Serial.printf("LEDs set up\n");
}

std::deque<Spectrum> spectrumHistory;

uint8_t yToHue(int y, const DisplayConfig &c)
{
    if (y > 6)
        return c.highHue;
    if (y > 4)
        return c.midHue;
    return c.lowHue;
}

void drawSingleBar(int x, int upto, const DisplayConfig &config, int briOverride = -1)
{
    for (int y = 0; y < upto; y++)
    {
        auto hue = yToHue(y, config);
        auto bri = config.brightness;
        if (briOverride != -1)
            bri = briOverride;
        auto color = CHSV(hue, SATURATION, bri);
        leds[XYsafe(x, y)] = color;
    }
}

void drawBars(const Spectrum &spectrum, const DisplayConfig &config)
{
    for (int x = 0; x < spectrum.size(); x++)
    {
        drawSingleBar(x, spectrum[x], config);
    }
}

void drawBarsWithBri(const Spectrum &spectrum, const DisplayConfig &config, int briOverride)
{
    for (int x = 0; x < spectrum.size(); x++)
    {
        drawSingleBar(x, spectrum[x], config, briOverride);
    }
}

void LEDSpectrumRenderer::render(const Spectrum &newSpectrum, const DisplayConfig &config)
{
    FastLED.clear(false);
    
    const uint8_t configuredHistoLength = config.histoLength;
    // Last frame is last in his
    spectrumHistory.push_back(newSpectrum);
    while (spectrumHistory.size() > configuredHistoLength)
    {
        spectrumHistory.pop_front();
    }

    const int histSize = spectrumHistory.size();

    // Max brightness for histo is set to 90% of the max brightness
    const int maxHistBri = config.brightness * 0.9;

    const int briHistoStep = maxHistBri / histSize;
    int bri = briHistoStep;

    // Write hist bars
    for (int iHis = 0; iHis < histSize - 1; iHis++)
    {
        auto hbars = spectrumHistory[iHis];
        drawBarsWithBri(hbars, config, bri);
        bri += briHistoStep;
    }

    // Write lastest frame at full brightness
    drawBars(newSpectrum, config);
    FastLED.show();
}

void LEDSpectrumRenderer::turnOff(const DisplayConfig &conf)
{
    if (spectrumHistory.empty())
    {
        // Back to initial state
        FastLED.clear();
        topBottomColors();
        FastLED.show();
    }

    FastLED.clear();

    // Loop back + fade on recorded history
    Spectrum spectrumStart = spectrumHistory.back();
    int startBrightness = conf.brightness;
    for (int b = startBrightness; b >= 0; b -= 1)
    {
        delay(7);
        drawBarsWithBri(spectrumStart, conf, b);
        FastLED.show();
    }
    spectrumHistory.clear();

    // Back to initial state
    FastLED.clear();
    topBottomColors();
    FastLED.show();
}

uint16_t XY(uint8_t x, uint8_t y)
{
    uint16_t i;
    if (x & 0x01)
    {
        i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
    }
    else
    {
        i = kMatrixHeight * (kMatrixWidth - x) - (y + 1);
    }

    return i;
}

uint16_t XYsafe(uint8_t x, uint8_t y)
{
    if (x >= kMatrixWidth)
        return -1;
    if (y >= kMatrixHeight)
        return -1;
    return XY(x, y);
}