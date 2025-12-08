#include "led_spectrum_renderer.h"
#include <queue>

CRGB leds_plus_safety_pixel[NUM_LEDS + 1];
CRGB *const leds = leds_plus_safety_pixel + 1;

LEDSpectrumRenderer::LEDSpectrumRenderer()
{
}

void LEDSpectrumRenderer::setupLeds()
{
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    leds[XYsafe(0, 0)] = CRGB::Blue;
    leds[XYsafe(kMatrixWidth - 1, kMatrixHeight - 1)] = CRGB::Red;
    FastLED.show();
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

CHSV colorFromHueAndB(int y, int b, const DisplayConfig &c)
{
    auto hue = yToHue(y, c);
    return CHSV(hue, SATURATION, b);
}

void drawSingleBar(int x, int upto, int brightness, const DisplayConfig &c)
{
    for (int y = 0; y < upto; y++)
    {
        leds[XYsafe(x, y)] = colorFromHueAndB(y, brightness, c);
    }
}

void drawBars(const Spectrum &bars, int bri, const DisplayConfig &c)
{
    for (int x = 0; x < bars.size(); x++)
    {
        drawSingleBar(x, bars[x], bri, c);
    }
}

void LEDSpectrumRenderer::render(const Spectrum &newBars, const DisplayConfig &c)
{
    const long msStart = millis();
    const uint8_t configuredHistoLength = c.histoLength;
    // Last frame is last in his
    spectrumHistory.push_back(newBars);
    while (spectrumHistory.size() > configuredHistoLength)
    {
        spectrumHistory.pop_front();
    }

    const int histSize = spectrumHistory.size();

    // max brightness for histo is set to 80% of the max brightness
    const int maxHistBri = c.brightness * 0.8;

    const int briHistoStep = maxHistBri / histSize;
    int bri = briHistoStep;

    // Write hist bars
    for (int iHis = 0; iHis < histSize - 1; iHis++)
    {
        auto hbars = spectrumHistory[iHis];
        drawBars(hbars, bri, c);
        bri += briHistoStep;
    }

    // Write lastest frame at full brightness
    drawBars(newBars, c.brightness, c);
    const long msEnd = millis();
    // Serial.printf("Rendered in %dms\n", msEnd - msStart);
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