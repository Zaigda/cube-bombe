#pragma once
#include "Countdown.h"
#include "Board1/Enigma/EnigmaStation.h"
#include "Common/Inputs/Button.h"
#include "Common/Outputs/LedState.h"

// Where the whole bomb is: before the first Start, timer running, all solved (VICTOIRE), or timer at 0 (BOOM).
enum class GameState : uint8_t { Waiting, Running, Defused, Exploded };

// The bomb rules: timer, VICTOIRE, BOOM, and the global reset when the reset button is held GLOBAL_RESET_HOLD_MS.
class GameManager {
public:
    GameManager(Countdown &bombTimer, const Button &globalResetButton);

    void AddStation(EnigmaStation &station);

    void Begin(uint32_t now);

    void Update(uint32_t now);

    GameState GetState() const;

    bool IsOver() const;

    uint32_t GetTimeInStateMs(uint32_t now) const;

    uint32_t GetResetHoldMs(uint32_t now) const;

private:
    void UpdateResetHold(uint32_t now);

    void UpdateRunning(uint32_t now);

    bool IsAnyStationStarted() const;

    bool AreAllStationsSolved() const;

    void Reset(uint32_t now);

    void End(GameState endState, LedState stationLed, uint32_t now);

    void ChangeState(GameState newState, uint32_t now);

    static constexpr uint8_t MAX_STATIONS = 4;

    Countdown &countdown;
    const Button &resetButton;
    EnigmaStation *stations[MAX_STATIONS] = {};
    uint8_t stationCount = 0;

    GameState state = GameState::Waiting;
    uint32_t stateStartMs = 0;
    bool resetHeld = false;
    bool resetDone = false;
    uint32_t resetHoldStartMs = 0;
};
