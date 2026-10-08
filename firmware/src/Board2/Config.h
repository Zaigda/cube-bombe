#pragma once
#include <cstdint>

// The serial link to board 1: same speed on both boards.
namespace LinkTuning {
    constexpr uint32_t BAUD = 115200;
}

// GPIO numbers of board 2. Never use 0, 19, 20 (USB), 35 to 37 (PSRAM) or 43, 44 (COM port).
namespace Pins {
    constexpr uint8_t LINK_TX = 21;
    constexpr uint8_t LINK_RX = 1;
}
