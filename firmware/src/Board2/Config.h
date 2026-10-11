#pragma once
#include <cstdint>
#include <iterator>
#include "Common/Config.h"

// GPIO numbers of board 2. Avoid 0 (BOOT button). The checks below refuse USB, flash, PSRAM and COM port pins.
namespace Pins {
    constexpr uint8_t LINK_RX = 21;
    constexpr uint8_t LINK_TX = 47;
    constexpr uint8_t LEFT_MICROPHONE = 13;
    constexpr uint8_t RIGHT_MICROPHONE = 14;
    constexpr uint8_t START_BUTTON = 11;
    constexpr uint8_t STATUS_LED = 12;

    // Every pin above: a new pin goes here too, so that the checks below see it.
    constexpr uint8_t USED_PINS[] = {
        LINK_RX, LINK_TX, LEFT_MICROPHONE, RIGHT_MICROPHONE, START_BUTTON, STATUS_LED
    };

    static_assert(!PinCheck::HasDuplicatePin(USED_PINS, std::size(USED_PINS)), "two things use the same pin");
    static_assert(!PinCheck::HasReservedPin(USED_PINS, std::size(USED_PINS)),
                  "a pin in USED_PINS is reserved or does not exist: USB, flash, PSRAM or COM port");
}

// How a microphone detects a sound. It reads a sample every SAMPLE_US (4 kHz, without blocking the serial link) and
// measures the sound level (RMS, in ADC counts) of each WINDOW_MS window. At startup, CALIBRATION_WINDOWS (2 s, keep
// quiet) learn the background noise. A sound is detected after ATTACK_WINDOWS windows in a row (100 ms) above the
// threshold: NOISE_RATIO times the background, and at least MIN_MARGIN above it. It is released after RELEASE_WINDOWS
// windows in a row (250 ms) under RELEASE_RATIO of the way from the background to the threshold. While nothing is
// heard, the background follows the level by BACKGROUND_FOLLOW of the gap each window. A sound held MAX_HOLD_MS is a
// lasting noise: it is released, and the microphone stays deaf while CALIBRATION_WINDOWS learn the new background.
namespace SoundTuning {
    constexpr uint32_t SAMPLE_US = 250;
    constexpr uint32_t WINDOW_MS = 50;
    constexpr uint32_t CALIBRATION_WINDOWS = 40;
    constexpr float NOISE_RATIO = 2.0f;
    constexpr float MIN_MARGIN = 15.0f;
    constexpr uint8_t ATTACK_WINDOWS = 2;
    constexpr uint8_t RELEASE_WINDOWS = 5;
    constexpr float RELEASE_RATIO = 0.6f;
    constexpr float BACKGROUND_FOLLOW = 0.02f;
    constexpr uint32_t MAX_HOLD_MS = 4000;
}
