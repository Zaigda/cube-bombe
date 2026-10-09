#include "BombDisplay.h"
#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "Board1/Config.h"
#include "Palette.h"

static constexpr uint32_t RINGS_START_MS = EndTuning::BOOM_FLASH_MS;
static constexpr uint32_t TEXT_START_MS = RINGS_START_MS + EndTuning::BOOM_RINGS_MS;
static constexpr uint32_t BOOM_CYCLE_MS = TEXT_START_MS + EndTuning::BOOM_TEXT_MS;
static constexpr uint16_t BOOM_RING_COLORS[] = {Palette::YELLOW, Palette::ORANGE, Palette::RED};
static constexpr uint16_t VICTORY_RING_COLORS[] = {Palette::WHITE, Palette::CYAN, Palette::GREEN};

static uint16_t GetRingColor(int radius, int maxRadius, const uint16_t *ringColors) {
    if (radius * 3 < maxRadius) {
        return ringColors[0];
    }
    if (radius * 3 < maxRadius * 2) {
        return ringColors[1];
    }
    return ringColors[2];
}

BombDisplay::BombDisplay(const GameManager &gameManager, Screen &resetHoldScreen)
    : game(gameManager), resetScreen(resetHoldScreen) {
}

void BombDisplay::AddScreen(Screen &screen) {
    if (screenCount >= MAX_SCREENS) {
        Serial.println("BombDisplay: too many screens, increase MAX_SCREENS");
        return;
    }
    screens[screenCount] = &screen;
    screenCount++;
}

void BombDisplay::Draw(uint32_t now) {
    if (game.IsOver()) {
        for (uint8_t i = 0; i < screenCount; i++) {
            DrawEnd(*screens[i], now);
        }
    }

    const uint32_t holdMs = game.GetResetHoldMs(now);
    if (holdMs >= GameTuning::RESET_FEEDBACK_AFTER_MS) {
        DrawResetHold(holdMs);
    }
}

void BombDisplay::DrawEnd(Screen &screen, uint32_t now) const {
    const uint32_t endMs = game.GetTimeInStateMs(now);
    screen.fillScreen(Palette::BLACK);
    if (game.GetState() == GameState::Defused) {
        DrawVictory(screen, endMs, game.GetRemainingMs(now));
    } else {
        DrawBoom(screen, endMs);
    }
}

void BombDisplay::DrawVictory(Screen &screen, uint32_t endMs, uint32_t remainingMs) const {
    if (endMs < RINGS_START_MS) {
        screen.fillScreen(Palette::GREEN);
    } else if (endMs < TEXT_START_MS) {
        DrawRings(screen, endMs - RINGS_START_MS, VICTORY_RING_COLORS);
    } else {
        const bool scrolling = DrawScrollingVictory(screen, endMs - TEXT_START_MS);
        if (!scrolling) {
            DrawVictoryTime(screen, remainingMs);
        }
    }
}

bool BombDisplay::DrawScrollingVictory(Screen &screen, uint32_t scrollMs) const {
    screen.setTextSize(EndTuning::VICTORY_TEXT_SIZE);
    screen.setTextWrap(false);
    int16_t x;
    int16_t y;
    uint16_t textWidth;
    uint16_t textHeight;
    screen.getTextBounds(Texts::VICTORY, 0, 0, &x, &y, &textWidth, &textHeight);
    const uint32_t scrolledLeds = scrollMs / EndTuning::VICTORY_SCROLL_STEP_MS;
    const bool scrolling = scrolledLeds < static_cast<uint32_t>(screen.width() + textWidth);

    if (scrolling) {
        const int left = screen.width() - static_cast<int>(scrolledLeds);
        screen.setTextColor(Palette::GREEN);
        for (int band = 0; band < screen.height(); band += Panels::PANEL_HEIGHT) {
            screen.setCursor(left, band + (Panels::PANEL_HEIGHT - textHeight) / 2);
            screen.print(Texts::VICTORY);
        }
    }
    screen.setTextSize(1);
    screen.setTextWrap(true);
    return scrolling;
}

void BombDisplay::DrawVictoryTime(Screen &screen, uint32_t remainingMs) const {
    for (int16_t band = 0; band < screen.height(); band += Panels::PANEL_HEIGHT) {
        screen.DrawCenteredText(Texts::VICTORY, Palette::GREEN, band + StationTuning::START_TEXT_TOP);
        screen.setTextSize(StationTuning::TIMER_TEXT_SIZE);
        screen.DrawCenteredTime(remainingMs, EndTuning::VICTORY_TIME_COLOR, band + StationTuning::TIMER_TEXT_TOP);
        screen.setTextSize(1);
    }
}

void BombDisplay::DrawBoom(Screen &screen, uint32_t boomMs) const {
    const uint32_t cycleMs = boomMs % BOOM_CYCLE_MS;
    if (cycleMs < RINGS_START_MS) {
        screen.fillScreen(Palette::WHITE);
    } else if (cycleMs < TEXT_START_MS) {
        DrawRings(screen, cycleMs - RINGS_START_MS, BOOM_RING_COLORS);
    } else {
        DrawBlinkingBoom(screen, cycleMs - TEXT_START_MS);
    }
}

void BombDisplay::DrawRings(Screen &screen, uint32_t ringsMs, const uint16_t *ringColors) const {
    const int centerX = screen.width() / 2;
    const int centerY = screen.height() / 2;
    const int cornerRadius = static_cast<int>(std::hypot(centerX, centerY));
    const int travel = cornerRadius + (EndTuning::BOOM_RING_COUNT - 1) * EndTuning::BOOM_RING_GAP;
    const int frontRadius = static_cast<int>(travel * ringsMs / EndTuning::BOOM_RINGS_MS);

    for (int ring = 0; ring < EndTuning::BOOM_RING_COUNT; ring++) {
        const int radius = frontRadius - ring * EndTuning::BOOM_RING_GAP;
        if (radius > 0) {
            const uint16_t color = GetRingColor(radius, cornerRadius, ringColors);
            screen.drawCircle(centerX, centerY, radius, color);
            screen.drawCircle(centerX, centerY, radius - 1, color);
        }
    }
}

void BombDisplay::DrawBlinkingBoom(Screen &screen, uint32_t textMs) const {
    if ((textMs / EndTuning::BOOM_BLINK_MS) % 2 != 0) {
        return;
    }
    screen.setTextSize(EndTuning::BOOM_TEXT_SIZE);
    screen.DrawCenteredText(Texts::BOOM, Palette::RED);
    screen.setTextSize(1);
}

void BombDisplay::DrawResetHold(uint32_t holdMs) {
    const bool done = holdMs >= GameTuning::GLOBAL_RESET_HOLD_MS;
    const int middle = resetScreen.height() / 2;
    const int barTop = middle + 2;
    const int barWidth = resetScreen.width() - 2 * EndTuning::RESET_BAR_MARGIN;
    const uint32_t shownMs = std::min(holdMs, GameTuning::GLOBAL_RESET_HOLD_MS);
    const int filledWidth = static_cast<int>(barWidth * shownMs / GameTuning::GLOBAL_RESET_HOLD_MS);

    resetScreen.fillScreen(Palette::BLACK);
    resetScreen.DrawCenteredText(done ? Texts::RESET_DONE : Texts::RESET, Palette::WHITE, middle - 10);
    resetScreen.drawRect(EndTuning::RESET_BAR_MARGIN, barTop, barWidth, EndTuning::RESET_BAR_HEIGHT, Palette::WHITE);
    resetScreen.fillRect(EndTuning::RESET_BAR_MARGIN, barTop, filledWidth, EndTuning::RESET_BAR_HEIGHT,
                         done ? Palette::GREEN : Palette::ORANGE);
}
