#pragma once
#include <Arduino.h>
#include <string>

#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

struct DisplayConfig
{
    uint8_t histoLength = 25;
    uint8_t brightness = 50;
    uint8_t lowHue = 96;
    uint8_t midHue = 64;
    uint8_t highHue = 0;
};

#endif