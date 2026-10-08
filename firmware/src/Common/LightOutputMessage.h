#pragma once

#include <Arduino.h>

struct LightOutputMessage
{
    uint16_t ledMask;
    uint8_t ledState;
};
