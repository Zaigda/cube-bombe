#pragma once
#include "Enigma.h"
#include "Board1/Core/Countdown.h"
#include "Board1/Outputs/Screen.h"
#include "Common/Inputs/Button.h"
#include "Common/Outputs/LedState.h"

// One enigma with its Start/Reset button, LED state and texts: "EXPLOSION" with the timer and "x/3" while it waits
// for its Start, "RATE" after a mistake, "PHASE DESAMORCEE x/3" once solved (x = enigmas solved); frozen at the end.
// In the last seconds, while the enigma is not in play (waiting or solved), IsInFinalAlarm() is true and its screens
// blink a red border.
class EnigmaStation {
public:
    EnigmaStation(const char *stationName, Enigma &stationEnigma, const Button &startButton,
                  const Countdown &bombTimer);

    void AddScreen(Screen &screen);

    void Update(uint32_t now);

    void Draw(uint32_t now);

    void Reset(uint32_t now);

    void Freeze(LedState endLed);

    bool IsStarted() const;

    bool IsPlaying() const;

    bool IsSolved() const;

    LedState GetLedState() const;

    bool IsInFinalAlarm(uint32_t now) const;

    static bool IsFinalAlarmLit(uint32_t now);

private:
    enum class Phase : uint8_t { Paused, Playing, Failed, Solved };

    void ReadStartButton(uint32_t now);

    void UpdateEnigma(uint32_t now);

    void DrawTexts(Screen &screen, uint32_t now) const;

    void DrawTimer(Screen &screen, uint32_t now) const;

    void DrawSolved(Screen &screen, uint16_t color) const;

    static void DrawProgress(Screen &screen, uint16_t color, int16_t top);

    void DrawFinalAlarm(Screen &screen, uint32_t now) const;

    static uint8_t stationTotal;
    static uint8_t solvedTotal;

    static constexpr uint8_t MAX_SCREENS = 2;

    const char *name;
    Enigma &enigma;
    const Button &start;
    const Countdown &timer;
    Screen *screens[MAX_SCREENS] = {};
    uint8_t screenCount = 0;

    Phase phase = Phase::Paused;
    LedState ledState = LedState::Off;
    uint32_t failedMs = 0;
    uint32_t solvedMs = 0;
    bool frozen = false;
};
