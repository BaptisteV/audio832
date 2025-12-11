#include "spectrum_websocket.h"
#include "perf_watcher.h"

Spectrum spectrum;
DisplayConfig conf;
SpectrumWebsocket *SpectrumWebsocket::instance = nullptr;
LEDSpectrumRenderer SpectrumWebsocket::spectrumRenderer;

const int PORT = 81;

SpectrumWebsocket::SpectrumWebsocket() : webSocket(PORT)
{
    instance = this;
}

void SpectrumWebsocket::start()
{
    spectrumRenderer.setupLeds();
    Serial.println("Wifi connecting...");
    WiFi.begin("Plop", "$Plop123");
    // check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED)
    {
    }
    Serial.printf("Wifi connected, starting ws://%s:%d\n", WiFi.localIP().toString().c_str(), PORT);
    Serial.println("WebSocket server starting");
    webSocket.onEvent(onMessageReceived);
    webSocket.begin();
    Serial.println("WebSocket server started");
}

void SpectrumWebsocket::loop() { webSocket.loop(); }

Spectrum unpackBinaryData(const uint8_t *mem, uint32_t len)
{
    Spectrum a;
    // Unpack two 4-bit values from each byte
    for (uint32_t i = 0; i < len; ++i)
    {
        uint8_t packedByte = mem[i];
        a[2 * i] = (packedByte >> 4) & 0x0F; // Extract the higher 4 bits
        a[2 * i + 1] = packedByte & 0x0F;    // Extract the lower 4 bits
    }

    return a;
}

void SpectrumWebsocket::onNewBarsReceived(uint8_t *payload, size_t length)
{
    spectrum = unpackBinaryData(payload, length);
    spectrumRenderer.render(spectrum, conf);
}

void SpectrumWebsocket::onNewDisplayConfigReceived(uint8_t *payload)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    conf.brightness = doc["Brightness"].as<uint8_t>();
    conf.highHue = doc["HighHue"].as<uint8_t>();
    conf.midHue = doc["MidHue"].as<uint8_t>();
    conf.lowHue = doc["LowHue"].as<uint8_t>();
    conf.histoLength = doc["HistoLength"].as<uint8_t>();
}

void SpectrumWebsocket::onMessageReceived(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (instance == nullptr)
        return;

    if (type == WStype_ERROR)
        Serial.printf("Websocket error\n");

    if (type == WStype_CONNECTED)
        Serial.printf("[%u] Client connected!\n", num);

    if (type == WStype_DISCONNECTED)
    {
        Serial.printf("[%u] Client disconnected!\n", num);
        spectrumRenderer.turnOff(conf);
    }
    if (type == WStype_BIN && length == 16)
        instance->onNewBarsReceived(payload, length);

    if (type == WStype_TEXT)
        instance->onNewDisplayConfigReceived(payload);
}