#pragma once
#include <cstdint>

// GPIO numbers of board 2. Never use 0, 19, 20 (USB), 35 to 37 (PSRAM) or 43, 44 (COM port).
namespace Pins {
    constexpr uint8_t LINK_RX = 21;
    constexpr uint8_t LINK_TX = 1;
    constexpr uint8_t INPUT_LEFT_MICRO_ANALOG = 13;
    constexpr uint8_t INPUT_RIGHT_MICRO_ANALOG = 14;
    constexpr uint8_t INPUT_RESET_BUTTON = 4;
}

namespace SoundTuning {
    constexpr uint32_t SAMPLE_US = 250; // Target 4 kHz, without blocking the serial link.
    constexpr uint32_t WINDOW_MS = 50;
    constexpr uint32_t CALIBRATION_WINDOWS = 40; // Two seconds without shouting at startup.
    constexpr float NOISE_RATIO = 2.0f;
    constexpr float MIN_MARGIN = 15.0f; // RMS ADC counts above background; tune on hardware.
    constexpr uint8_t ATTACK_WINDOWS = 2; // 100 ms before triggering.
    constexpr uint8_t RELEASE_WINDOWS = 5; // 250 ms before clearing.
}
