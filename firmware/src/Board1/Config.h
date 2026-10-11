#pragma once
#include <cstdint>
#include <iterator>
#include "Board1/Core/Palette.h"
#include "Common/Config.h"

// The whole game: one timer for the 3 enigmas, started by the first Start, red under FINAL_WARNING_MS. Under
// FINAL_ALARM_MS, the screens of the enigmas not in play (DEPART or DESAMORCE) get a red border FINAL_ALARM_BORDER
// LEDs thick, blinking every FINAL_ALARM_BLINK_MS (keep a multiple of LinkTuning::SEND_MS); so do the 9 grid LEDs of
// the light enigma. Holding the snake Start/Reset button GLOBAL_RESET_HOLD_MS resets everything ("RESET" shows after
// RESET_FEEDBACK_AFTER_MS). Texts::START pulses from PULSE_MIN_LEVEL to full white every PULSE_MS.
namespace GameTuning {
    constexpr uint32_t GAME_DURATION_MS = (9 * 60 + 56) * 1000;
    constexpr uint32_t FINAL_WARNING_MS = 30 * 1000;
    constexpr uint32_t FINAL_ALARM_MS = 10 * 1000;
    constexpr uint32_t FINAL_ALARM_BLINK_MS = 250;
    constexpr int16_t FINAL_ALARM_BORDER = 2;
    constexpr uint32_t GLOBAL_RESET_HOLD_MS = 8000;
    constexpr uint32_t RESET_FEEDBACK_AFTER_MS = 2000;
    constexpr uint32_t PULSE_MS = 2000;
    constexpr uint8_t PULSE_MIN_LEVEL = 120;

    static_assert(PULSE_MS >= 2, "PULSE_MS must be at least 2");
    static_assert(FINAL_ALARM_BLINK_MS > 0, "FINAL_ALARM_BLINK_MS cannot be 0");
}

// The end of the game. BOOM loops on every screen: a white flash, rings growing from the middle (yellow, orange,
// then red), then "BOOM" blinking. VICTOIRE plays once with the same timings: a green flash, rings (white, cyan,
// then green), then "VICTOIRE" in VICTORY_TEXT_SIZE crossing every panel once from right to left, one LED every
// VICTORY_SCROLL_STEP_MS. Then every panel stays on "VICTOIRE" in green with the time that was left under it in
// VICTORY_TIME_COLOR, placed like Texts::START and its timer, until the global reset. The grid of board 3 runs one LED
// along CHASE_ORDER (VICTOIRE) or blinks every LED at StatusLight::FAST_BLINK_MS (BOOM). Grid order: 0 1 2 / 3 4 5 /
// 6 7 8. Keep CHASE_STEP_MS a multiple of LinkTuning::SEND_MS, or the chase stutters. The RESET bar (global reset
// held) is in LEDs; it turns green with "PRET" once the reset is done, until the button is released.
namespace EndTuning {
    constexpr uint32_t BOOM_FLASH_MS = 150;
    constexpr uint32_t BOOM_RINGS_MS = 1200;
    constexpr int BOOM_RING_COUNT = 3;
    constexpr int BOOM_RING_GAP = 6;
    constexpr uint32_t BOOM_TEXT_MS = 2400;
    constexpr uint32_t BOOM_BLINK_MS = 300;
    constexpr uint8_t BOOM_TEXT_SIZE = 2;
    constexpr uint8_t VICTORY_TEXT_SIZE = 3;
    constexpr uint32_t VICTORY_SCROLL_STEP_MS = 20;
    constexpr uint16_t VICTORY_TIME_COLOR = Palette::WHITE;
    constexpr int RESET_BAR_MARGIN = 4;
    constexpr int RESET_BAR_HEIGHT = 6;
    constexpr uint32_t CHASE_STEP_MS = 150;
    constexpr uint8_t CHASE_ORDER[] = {0, 1, 2, 5, 8, 7, 6, 3};

    static_assert(BOOM_RINGS_MS > 0 && BOOM_BLINK_MS > 0 && CHASE_STEP_MS > 0 && VICTORY_SCROLL_STEP_MS > 0,
                  "these durations cannot be 0");
}

// Every text shown on the panels: French, uppercase, no accents (the panel font has none). At text size 1 a
// character is 6 LEDs wide, so 10 characters at most on a 64-LED panel.
namespace Texts {
    constexpr const char *START = "EXPLOSION";
    constexpr const char *FAILED = "RATE";
    constexpr const char *SOLVED = "PHASE";
    constexpr const char *SOLVED_LINE_2 = "DESAMORCEE";
    constexpr const char *PLUG = "BRANCHE";
    constexpr const char *VICTORY = "VICTOIRE";
    constexpr const char *BOOM = "BOOM";
    constexpr const char *RESET = "RESET";
    constexpr const char *RESET_DONE = "PRET";
    constexpr const char *BOARD_2 = "CARTE 2";
    constexpr const char *BOARD_3 = "CARTE 3";
}

// Snake gameplay values (enigma 1), from the GD sheet. Positions and lengths are in LEDs, y counted from the top.
// Up to TURN_QUEUE_SIZE turn presses wait in line; the snake takes one of them per step.
namespace SnakeTuning {
    constexpr int CELL_SIZE = 2;
    constexpr uint32_t STEP_MS = 250;
    constexpr int TURN_QUEUE_SIZE = 2;
    constexpr int START_X = 16;
    constexpr int START_Y = 48;
    constexpr int LEVEL_LENGTHS[3] = {16, 32, 48};
    constexpr int FIRST_APPLE_MIN_X = 32;
    constexpr int FIRST_APPLE_MAX_X = 48;
    constexpr int FIRST_APPLE_MIN_Y = 16;
    constexpr int FIRST_APPLE_MAX_Y = 32;
    constexpr int APPLE_SIZE = 4;
    constexpr int MOUTH_SIZE = 4;
    constexpr uint32_t MOUTH_MS = 250;

    static_assert(TURN_QUEUE_SIZE >= 1, "TURN_QUEUE_SIZE must be at least 1");
}

// Space invaders gameplay values (enigma 2), from the GD sheet. In LEDs. An alien reaching the ship sets the kill
// counter back to 0. The guide sees the counter "kills/ALIEN_COUNT" in gray (COUNTER_LEVEL, 0-255) in a corner.
// For HIT_FLASH_MS after a hit, the guide sees the hit alien in orange and the counter in white.
// WRAP_AROUND = true: the ship leaving one side comes back on the other, so one microphone is enough to play.
namespace InvadersTuning {
    constexpr int SHIP_WIDTH = 8;
    constexpr int SHIP_HEIGHT = 3;
    constexpr int SIDE_MARGIN = 4;
    constexpr bool WRAP_AROUND = true;
    constexpr uint32_t SHIP_STEP_MS = 40;
    constexpr uint32_t FIRE_MS = 500;
    constexpr uint32_t BULLET_STEP_MS = 20;
    constexpr int BULLET_HEIGHT = 2;
    constexpr int ALIEN_SIZE = 8;
    constexpr int COLUMN_COUNT = 7;
    constexpr int FIRST_COLUMN_X = 4;
    constexpr uint32_t ALIEN_STEP_MS = 500;
    constexpr int ALIEN_COUNT = 5;
    constexpr int16_t COUNTER_LEFT = 1;
    constexpr int16_t COUNTER_TOP = 1;
    constexpr uint8_t COUNTER_LEVEL = 180;
    constexpr uint32_t HIT_FLASH_MS = 300;
}

// Light enigma values (enigma 3), from the GD sheet. A hole is covered below COVERED_PERCENT of the brightest light
// it had since Start; a value above MAX_LIGHT (the sensor's maximum) is a broken message and is ignored.
// Each pattern = the 6 holes to cover, drawn as the plate is seen (grid LED order 0 1 2 / 3 4 5 / 6 7 8), always
// coverable by 2 of the 3 pieces (L, L, bar of 3). Before Start, the grid LED IDLE_LED blinks
// at the pace of Texts::START (GameTuning::PULSE_MS). Covering any hole presses Start, so board 3 needs no Start
// button: before Start, each hole's light is compared to its brightest recent light, which follows a darker room by
// 1/IDLE_FOLLOW of the gap every IDLE_STEP_MS. The sensors are wired out of order: LED_OF_SENSOR[s] is the grid LED
// in the same hole as sensor s (found on the cube with TEST = true).
// SHOW_ALL_COVERED = true (debug): every covered hole lights its LED, pattern or not, and 9 covered holes stay lit.
namespace LightTuning {
    constexpr int HOLE_COUNT = 9;
    constexpr int IDLE_LED = 4;
    constexpr uint32_t IDLE_STEP_MS = 100;
    constexpr uint16_t IDLE_FOLLOW = 50;
    constexpr int COVERED_PERCENT = 35;
    constexpr int LED_OF_SENSOR[HOLE_COUNT] = {2, 5, 6, 1, 4, 7, 0, 3, 8};
    constexpr bool SHOW_ALL_COVERED = false;
    constexpr bool TEST = false;
    constexpr uint32_t TEST_LED_MS = 700;
    constexpr uint32_t TEST_PRINT_MS = 500;
    constexpr uint16_t MAX_LIGHT = 4095;
    constexpr int PATTERN_COUNT = 6;
    constexpr bool PATTERNS[PATTERN_COUNT][HOLE_COUNT] = {
        {1, 1, 0,
         1, 1, 1,
         0, 1, 0},
        {1, 0, 0,
         1, 1, 0,
         1, 1, 1},
        {0, 1, 1,
         1, 0, 1,
         1, 1, 0},
        {1, 1, 1,
         1, 0, 1,
         1, 0, 0},
        {1, 1, 1,
         1, 1, 0,
         0, 1, 0},
        {1, 1, 1,
         0, 1, 0,
         1, 1, 0},
    };

    static_assert(IDLE_LED >= 0 && IDLE_LED < HOLE_COUNT, "IDLE_LED must be a hole of the grid");
}

// What every enigma station shows around its game: "RATE" in FAILED_TEXT_SIZE for FAIL_MS, a green flash for
// SOLVED_FLASH_MS before "PHASE DESAMORCEE x/3" (three lines of TEXT_LINE LEDs; x = enigmas solved), and where
// "EXPLOSION" (WAITING_TEXT_TOP), the timer (WAITING_TIMER_TOP) and "x/3" (PROGRESS_TEXT_TOP) are written while the
// enigma waits for its Start (top in LEDs; text size 2 = 12 LEDs per character). The timer is TIMER_COLOR, then
// red under GameTuning::FINAL_WARNING_MS. Before the first Start, "CARTE 2" and "CARTE 3" show in red on the snake
// screen from LINK_CHECK_TOP (one line = LINK_CHECK_LINE LEDs) while that board sends nothing.
namespace StationTuning {
    constexpr uint32_t FAIL_MS = 1000;
    constexpr uint8_t FAILED_TEXT_SIZE = 2;
    constexpr uint32_t SOLVED_FLASH_MS = 400;
    constexpr int16_t START_TEXT_TOP = 4;
    constexpr int16_t TIMER_TEXT_TOP = 14;
    constexpr uint8_t TIMER_TEXT_SIZE = 2;
    constexpr uint16_t TIMER_COLOR = Palette::CYAN;
    constexpr int16_t WAITING_TEXT_TOP = 0;
    constexpr int16_t WAITING_TIMER_TOP = 9;
    constexpr int16_t PROGRESS_TEXT_TOP = 25;
    constexpr int16_t TEXT_LINE = 8;
    constexpr int16_t LINK_CHECK_TOP = 36;
    constexpr int16_t LINK_CHECK_LINE = 10;
}

// The chain of HUB75 panels. Panel 0 is the one plugged into the board. MIN_REFRESH_HZ is the library default:
// raise it (e.g. 200) to reduce flicker on camera, at the cost of color depth. At boot, each panel shows its number
// in TEST_TEXT_SIZE and a white square of TEST_SQUARE_SIZE LEDs in its top-left corner, to check the chain order
// and ROTATED_180.
namespace Panels {
    constexpr uint16_t PANEL_WIDTH = 64;
    constexpr uint16_t PANEL_HEIGHT = 32;
    constexpr uint8_t PANEL_COUNT = 4;
    constexpr uint8_t SNAKE_PANELS = 2;
    constexpr uint8_t GUIDE_PANEL = 3;
    constexpr uint8_t SINGER_PANEL = 2;
    constexpr uint8_t MAX_PANELS = 4;
    constexpr uint8_t BRIGHTNESS = 60;
    constexpr uint8_t MIN_REFRESH_HZ = 60;
    constexpr uint32_t FRAME_MS = 20;
    constexpr bool ROTATED_180[MAX_PANELS] = {true, true, false, false};
    constexpr uint8_t TEST_TEXT_SIZE = 3;
    constexpr int16_t TEST_SQUARE_SIZE = 4;

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

    constexpr uint8_t SNAKE_LEFT_BUTTON = 41;
    constexpr uint8_t SNAKE_RIGHT_BUTTON = 42;
    constexpr uint8_t SNAKE_RESET_BUTTON = 40;
    constexpr uint8_t STATUS_LED = 39;
    constexpr uint8_t CABLES[3] = {11, 12, 13};
    constexpr uint8_t PLUGS[3] = {48, 47, 45};
    constexpr uint8_t LINK_TX = 21;
    constexpr uint8_t LINK_RX = 1;
    constexpr uint8_t LIGHT_LINK_TX = 14;
    constexpr uint8_t LIGHT_LINK_RX = 2;

    // Every pin above (panels apart): a new pin goes here too, so that the checks below see it.
    constexpr uint8_t USED_PINS[] = {
        SNAKE_LEFT_BUTTON, SNAKE_RIGHT_BUTTON, SNAKE_RESET_BUTTON, STATUS_LED,
        CABLES[0], CABLES[1], CABLES[2],
        PLUGS[0], PLUGS[1], PLUGS[2],
        LINK_TX, LINK_RX, LIGHT_LINK_TX, LIGHT_LINK_RX
    };

    constexpr bool UsesPanelPin() {
        for (uint8_t pin : USED_PINS) {
            if (IsPanelPin(pin)) {
                return true;
            }
        }
        return false;
    }

    static_assert(!PinCheck::HasDuplicatePin(USED_PINS, std::size(USED_PINS)), "two things use the same pin");
    static_assert(!PinCheck::HasReservedPin(USED_PINS, std::size(USED_PINS)),
                  "a pin in USED_PINS is reserved or does not exist: USB, flash, PSRAM or COM port");
    static_assert(!UsesPanelPin(), "a pin in USED_PINS belongs to the panels");
}
