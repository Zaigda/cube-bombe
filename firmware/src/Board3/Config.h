#pragma once
#include <cstdint>
#include "Common/Messages/Enigma3Message.h"

// Each sensor value is the average of SAMPLES reads.
namespace LightTuning {
    constexpr int SAMPLES = 4;
}

// GPIO numbers of board 3. Grid order: left to right, top to bottom (0 1 2 / 3 4 5 / 6 7 8).
namespace Pins {
    constexpr uint8_t LIGHT_SENSORS[LIGHT_SENSOR_COUNT] = {18, 16, 8, 5, 15, 6, 7, 4, 17};
    constexpr uint8_t GRID_LEDS[LIGHT_SENSOR_COUNT] = {42, 2, 1, 41, 40, 47, 12, 11, 10};
    constexpr uint8_t START_BUTTON = 39;
    constexpr uint8_t STATUS_LED = 9;
    constexpr uint8_t LINK_TX = 21;
    constexpr uint8_t LINK_RX = 13;
}
