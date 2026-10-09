#pragma once
#include "GameManager.h"
#include "Board1/Outputs/Screen.h"

// Draws what belongs to the whole bomb, over the enigmas: the VICTOIRE or BOOM animation on every screen once the
// game is over, and "RESET" with a progress bar on one screen while the global reset button is held.
class BombDisplay {
public:
    BombDisplay(const GameManager &gameManager, Screen &resetHoldScreen);

    void AddScreen(Screen &screen);

    void Draw(uint32_t now);

private:
    void DrawEnd(Screen &screen, uint32_t endMs) const;

    void DrawVictory(Screen &screen, uint32_t endMs) const;

    void DrawScrollingVictory(Screen &screen, uint32_t scrollMs) const;

    void DrawBoom(Screen &screen, uint32_t boomMs) const;

    void DrawRings(Screen &screen, uint32_t ringsMs, const uint16_t *ringColors) const;

    void DrawBlinkingBoom(Screen &screen, uint32_t textMs) const;

    void DrawResetHold(uint32_t holdMs);

    static constexpr uint8_t MAX_SCREENS = 3;

    const GameManager &game;
    Screen &resetScreen;
    Screen *screens[MAX_SCREENS] = {};
    uint8_t screenCount = 0;
};
