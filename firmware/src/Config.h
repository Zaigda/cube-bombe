#pragma once
#include <cstdint>

// Game timing. Only used by GameManager, which is not wired yet.
namespace Timing {
    constexpr uint32_t GAME_DURATION_MS = 5 * 60 * 1000;
    constexpr uint32_t MISTAKE_PENALTY_MS = 15 * 1000;
    constexpr uint32_t RESULT_SCREEN_MS = 10 * 1000;
}

// Snake gameplay values (enigma 1).
namespace SnakeTuning {
    constexpr int CELL_SIZE = 4;
    constexpr uint32_t STEP_MS = 300;
    constexpr int START_LENGTH = 2;
    constexpr int START_COLUMN = 3;
    constexpr int GROWTH_FACTOR = 2;
}

// The chain of HUB75 panels. Panel 0 is the one plugged into the board.
namespace Panels {
    constexpr uint16_t PANEL_WIDTH = 64;
    constexpr uint16_t PANEL_HEIGHT = 32;
    constexpr uint8_t PANEL_COUNT = 2;
    constexpr uint8_t SNAKE_PANELS = 2;
    constexpr uint8_t MAX_PANELS = 4;
    constexpr uint8_t BRIGHTNESS = 60;
    constexpr uint32_t FRAME_MS = 20;
    constexpr bool ROTATED_180[MAX_PANELS] = {false, true, false, false};

    static_assert(PANEL_COUNT >= 1 && PANEL_COUNT <= MAX_PANELS, "PANEL_COUNT must be between 1 and MAX_PANELS");
    static_assert(SNAKE_PANELS >= 1 && SNAKE_PANELS <= PANEL_COUNT, "SNAKE_PANELS must be between 1 and PANEL_COUNT");
}

// GPIO numbers. The HUB75 pins belong to the panels: never wire anything else to them.
namespace Pins {
    // The 14 signals of the panels' HUB75 connector (-1 = not wired).
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

    constexpr Hub75 HUB75 = {
        4, 5, 6,        // R1 G1 B1: colors, top half
        7, 15, 16,      // R2 G2 B2: colors, bottom half
        17, 18, 8, 3,   // A B C D: row select
        -1,             // E: only for 64-row panels
        46, 10, 9       // LAT OE CLK
    };

    constexpr bool IsPanelPin(int pin) {
        return pin == HUB75.r1 || pin == HUB75.g1 || pin == HUB75.b1 || pin == HUB75.r2 || pin == HUB75.g2 ||
               pin == HUB75.b2 || pin == HUB75.a || pin == HUB75.b || pin == HUB75.c || pin == HUB75.d ||
               pin == HUB75.e || pin == HUB75.lat || pin == HUB75.oe || pin == HUB75.clk;
    }

    // Panel pins, native USB (19, 20) and octal PSRAM (35 to 37).
    constexpr bool IsReservedPin(int pin) {
        return IsPanelPin(pin) || pin == 19 || pin == 20 || (pin >= 35 && pin <= 37);
    }

    constexpr uint8_t SNAKE_LEFT_BUTTON = 41;
    constexpr uint8_t SNAKE_RIGHT_BUTTON = 42;
    constexpr uint8_t START_BUTTON = 13;

    static_assert(!IsReservedPin(SNAKE_LEFT_BUTTON), "SNAKE_LEFT_BUTTON is on a reserved pin");
    static_assert(!IsReservedPin(SNAKE_RIGHT_BUTTON), "SNAKE_RIGHT_BUTTON is on a reserved pin");
    static_assert(!IsReservedPin(START_BUTTON), "START_BUTTON is on a reserved pin");
}
