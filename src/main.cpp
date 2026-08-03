#include <Arduino.h>
#include "spectrum_websocket.h"

SpectrumWebsocket spectrumWebsocket;

void setup()
{
    Serial.begin(115200);
    spectrumWebsocket.start();
}

void loop()
{
    spectrumWebsocket.loop();
    EVERY_N_MILLIS(5000)
    {
        Serial.print((int)FastLED.getFPS());
        Serial.println(" fps");
    }
}