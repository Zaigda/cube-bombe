#pragma once
#include <cstdint>

// Game timing. Only used by GameManager, which is not wired yet.
namespace Timing {
    constexpr uint32_t GAME_DURATION_MS = 5 * 60 * 1000;
    constexpr uint32_t MISTAKE_PENALTY_MS = 15 * 1000;
    constexpr uint32_t RESULT_SCREEN_MS = 10 * 1000;
}

// Snake gameplay values (enigma 1), from the GD sheet. Positions and lengths are in LEDs, y counted from the top.
namespace SnakeTuning {
    constexpr int CELL_SIZE = 2;
    constexpr uint32_t STEP_MS = 250;
    constexpr int START_X = 16;
    constexpr int START_Y = 48;
    constexpr int LEVEL_LENGTHS[3] = {16, 32, 48};
    constexpr int FIRST_APPLE_MIN_X = 32;
    constexpr int FIRST_APPLE_MAX_X = 48;
    constexpr int FIRST_APPLE_MIN_Y = 16;
    constexpr int FIRST_APPLE_MAX_Y = 32;
    constexpr int MOUTH_SIZE = 4;
    constexpr uint32_t MOUTH_MS = 250;
    constexpr uint32_t FAIL_MS = 1000;
    constexpr int16_t START_TEXT_TOP = 8;
}

// The status LED of the enigma.
namespace StatusLight {
    constexpr uint32_t BLINK_MS = 400;
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
    constexpr uint8_t SNAKE_RESET_BUTTON = 40;
    constexpr uint8_t STATUS_LED = 39;
    constexpr uint8_t CABLES[3] = {11, 12, 13};
    constexpr uint8_t PLUGS[3] = {47, 48, 45};

    static_assert(!IsReservedPin(SNAKE_LEFT_BUTTON), "SNAKE_LEFT_BUTTON is on a reserved pin");
    static_assert(!IsReservedPin(SNAKE_RIGHT_BUTTON), "SNAKE_RIGHT_BUTTON is on a reserved pin");
    static_assert(!IsReservedPin(SNAKE_RESET_BUTTON), "SNAKE_RESET_BUTTON is on a reserved pin");
    static_assert(!IsReservedPin(STATUS_LED), "STATUS_LED is on a reserved pin");
    static_assert(!IsReservedPin(CABLES[0]) && !IsReservedPin(CABLES[1]) && !IsReservedPin(CABLES[2]),
                  "a cable is on a reserved pin");
    static_assert(!IsReservedPin(PLUGS[0]) && !IsReservedPin(PLUGS[1]) && !IsReservedPin(PLUGS[2]),
                  "a plug is on a reserved pin");
}
