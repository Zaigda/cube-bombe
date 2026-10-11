#pragma once
#include <cstdint>
#include "Common/Outputs/LedState.h"

constexpr uint8_t LIGHT_SENSOR_COUNT = 9;

// Board 3 to board 1: its Start/Reset button and the 9 light values, in the order of the sensor pins on board 3 (not
// the grid order: board 1 finds the hole of each sensor with LightTuning::LED_OF_SENSOR).
struct Enigma3InputMessage {
    bool startButton;
    uint16_t light[LIGHT_SENSOR_COUNT];
};

// Board 1 to board 3: the grid LEDs to light (bit i = grid LED i, grid order 0 1 2 / 3 4 5 / 6 7 8) and what the
// light status LED shows.
struct Enigma3OutputMessage {
    uint16_t ledMask;
    LedState ledState;
};
