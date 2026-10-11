#pragma once
#include <cstdint>
#include <iterator>
#include "Common/Config.h"

// GPIO numbers of board 2. Avoid 0 (BOOT button). The checks below refuse USB, flash, PSRAM and COM port pins.
namespace Pins {
    constexpr uint8_t LINK_RX = 21;
    constexpr uint8_t LINK_TX = 47;
    constexpr uint8_t INPUT_LEFT_MICRO_ANALOG = 13;
    constexpr uint8_t INPUT_RIGHT_MICRO_ANALOG = 14;
    constexpr uint8_t INPUT_RESET_BUTTON = 11;
    constexpr uint8_t OUTPUT_GAME_STATE_LED = 12;

    // Every pin above: a new pin goes here too, so that the checks below see it.
    constexpr uint8_t USED_PINS[] = {
        LINK_RX, LINK_TX, INPUT_LEFT_MICRO_ANALOG, INPUT_RIGHT_MICRO_ANALOG, INPUT_RESET_BUTTON, OUTPUT_GAME_STATE_LED
    };

    static_assert(!PinCheck::HasDuplicatePin(USED_PINS, std::size(USED_PINS)), "two things use the same pin");
    static_assert(!PinCheck::HasReservedPin(USED_PINS, std::size(USED_PINS)),
                  "a pin in USED_PINS is reserved or does not exist: USB, flash, PSRAM or COM port");
}

namespace SoundTuning {
    constexpr uint32_t SAMPLE_US = 250; // Target 4 kHz, without blocking the serial link.
    constexpr uint32_t WINDOW_MS = 50;
    constexpr uint32_t CALIBRATION_WINDOWS = 40; // Two seconds without shouting at startup.
    constexpr float NOISE_RATIO = 2.0f;
    constexpr float MIN_MARGIN = 15.0f; // RMS ADC counts above background; tune on hardware.
    constexpr uint8_t ATTACK_WINDOWS = 2; // 100 ms before triggering.
    constexpr uint8_t RELEASE_WINDOWS = 5; // 250 ms before clearing.
    constexpr uint32_t MAX_HOLD_MS = 4000; // Then released, deaf while CALIBRATION_WINDOWS learn the new background.
}
