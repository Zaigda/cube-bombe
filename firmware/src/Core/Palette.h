#pragma once
#include <cstdint>

namespace Palette {
    constexpr uint16_t Rgb(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    constexpr uint16_t BLACK = Rgb(0, 0, 0);
    constexpr uint16_t WHITE = Rgb(255, 255, 255);
    constexpr uint16_t RED = Rgb(255, 0, 0);
    constexpr uint16_t GREEN = Rgb(0, 255, 0);
    constexpr uint16_t BLUE = Rgb(0, 0, 255);
    constexpr uint16_t YELLOW = Rgb(255, 255, 0);
    constexpr uint16_t ORANGE = Rgb(255, 120, 0);
}