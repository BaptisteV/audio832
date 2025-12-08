#pragma once
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "display_config.h"
#include "led_spectrum_renderer.h"

class SpectrumWebsocket
{
public:
    SpectrumWebsocket();
    void start();
    void loop();

private:
    WebSocketsServer webSocket;
    LEDSpectrumRenderer spectrumRenderer;
    static SpectrumWebsocket *instance;
    static void onMessageReceived(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    void onNewBarsReceived(uint8_t *payload, size_t length);
    void onNewDisplayConfigReceived(uint8_t *payload);
};
