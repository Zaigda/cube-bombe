#pragma once

#include <Arduino.h>

constexpr uint8_t LIGHT_SENSOR_COUNT = 9;

struct Enigma3InputMessage
{
    bool resetBtn;
    uint16_t light[LIGHT_SENSOR_COUNT];
};

struct Enigma3OutputMessage
{
    uint16_t ledMask;
    uint8_t ledState;
};
