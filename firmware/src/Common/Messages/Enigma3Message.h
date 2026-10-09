#pragma once
#include <Arduino.h>
#include "Common/Outputs/LedState.h"

constexpr uint8_t LIGHT_SENSOR_COUNT = 9;

// Board 3 to board 1: its Start/Reset button and the 9 light values (grid order).
struct Enigma3InputMessage {
    bool resetBtn;
    uint16_t light[LIGHT_SENSOR_COUNT];
};

// Board 1 to board 3: the grid LEDs to light (bit i = LED i) and what the light status LED shows.
struct Enigma3OutputMessage {
    uint16_t ledMask;
    LedState ledState;
};
