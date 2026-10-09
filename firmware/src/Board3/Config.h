#pragma once
#include <cstdint>
#include <iterator>
#include "Common/Config.h"
#include "Common/Messages/Enigma3Message.h"

// Each sensor value is the average of SAMPLES reads.
namespace LightTuning {
    constexpr int SAMPLES = 4;
}

// At startup, the grid LEDs light one at a time in grid order, SWEEP_STEP_MS each, to check the wiring.
namespace GridTuning {
    constexpr uint32_t SWEEP_STEP_MS = 150;
}

// GPIO numbers of board 3. Grid order: left to right, top to bottom (0 1 2 / 3 4 5 / 6 7 8).
// Avoid 0 (BOOT button). The checks below refuse USB, flash, PSRAM and COM port pins.
namespace Pins {
    constexpr uint8_t LIGHT_SENSORS[LIGHT_SENSOR_COUNT] = {18, 16, 8, 5, 15, 6, 7, 4, 17};
    constexpr uint8_t GRID_LEDS[LIGHT_SENSOR_COUNT] = {42, 2, 1, 41, 40, 47,  38, 39, 45};
    constexpr uint8_t START_BUTTON = 10;
    constexpr uint8_t STATUS_LED = 9;
    constexpr uint8_t LINK_TX = 21;
    constexpr uint8_t LINK_RX = 13;

    // Every pin above: a new pin goes here too, so that the checks below see it.
    constexpr uint8_t USED_PINS[] = {
        LIGHT_SENSORS[0], LIGHT_SENSORS[1], LIGHT_SENSORS[2],
        LIGHT_SENSORS[3], LIGHT_SENSORS[4], LIGHT_SENSORS[5],
        LIGHT_SENSORS[6], LIGHT_SENSORS[7], LIGHT_SENSORS[8],
        GRID_LEDS[0], GRID_LEDS[1], GRID_LEDS[2],
        GRID_LEDS[3], GRID_LEDS[4], GRID_LEDS[5],
        GRID_LEDS[6], GRID_LEDS[7], GRID_LEDS[8],
        START_BUTTON, STATUS_LED, LINK_TX, LINK_RX
    };

    static_assert(LIGHT_SENSOR_COUNT == 9, "USED_PINS lists 9 sensors and 9 LEDs: update it");
    static_assert(!PinCheck::HasDuplicatePin(USED_PINS, std::size(USED_PINS)), "two things use the same pin");
    static_assert(!PinCheck::HasReservedPin(USED_PINS, std::size(USED_PINS)),
                  "a pin in USED_PINS is reserved or does not exist: USB, flash, PSRAM or COM port");
}
