#pragma once

#include <Arduino.h>

constexpr uint8_t LIGHT_SENSOR_COUNT = 9;

struct LightInputMessage
{
    bool resetBtn;
    uint16_t light[LIGHT_SENSOR_COUNT];
};
