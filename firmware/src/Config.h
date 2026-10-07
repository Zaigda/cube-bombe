#pragma once
#include <cstdint>

namespace Timing {
    constexpr uint32_t GAME_DURATION_MS = 5 * 60 * 1000;
    constexpr uint32_t MISTAKE_PENALTY_MS = 15 * 1000;
    constexpr uint32_t RESULT_SCREEN_MS = 10 * 1000;
}

namespace SnakeTuning {
    constexpr int CELL_SIZE = 4;
    constexpr uint32_t STEP_MS = 300;
    constexpr int START_LENGTH = 2;
    constexpr int START_COLUMN = 3;
    constexpr int GROWTH_FACTOR = 2;
}

namespace Panels {
    constexpr uint16_t PANEL_WIDTH = 64;
    constexpr uint16_t PANEL_HEIGHT = 32;
    constexpr uint8_t MAX_PANELS = 4;
    constexpr uint8_t BRIGHTNESS = 60;
    constexpr uint32_t FRAME_MS = 20;
    constexpr bool ROTATED_180[MAX_PANELS] = {false, true, false, false};
}

namespace Pins {
    struct Hub75 {
        int8_t r1;
        int8_t g1;
        int8_t b1;
        int8_t r2;
        int8_t g2;
        int8_t b2;
        int8_t a;
        int8_t b;
        int8_t c;
        int8_t d;
        int8_t e;
        int8_t lat;
        int8_t oe;
        int8_t clk;
    };

    constexpr Hub75 HUB75 = {15, 16, 17, 18, 11, 12, 13, 14, 39, 40, -1, 41, 42, 21};

    constexpr uint8_t SNAKE_LEFT_BUTTON = 4;
    constexpr uint8_t SNAKE_RIGHT_BUTTON = 5;
    constexpr uint8_t START_BUTTON = 6;
}
