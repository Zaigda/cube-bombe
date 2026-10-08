#pragma once
#include "BombCode.h"
#include "Countdown.h"
#include "Board1/Enigma/Enigma.h"
#include "Board1/Inputs/Button.h"
#include "Board1/Outputs/Screen.h"

enum class GameState : uint8_t { Waiting, Playing, Defused, Exploded };

// A whole game, from waiting to defused or exploded. Not wired yet: to be rewritten for 3 boards.
class GameManager {
public:
    GameManager(Countdown &gameCountdown, const Button &startButton, BombCode &bombCode);

    void AddScreen(Screen &screen);

    void AddEnigma(Enigma &enigma);

    void Update(uint32_t now);

    void Draw(uint32_t now);

    GameState GetState() const;

private:
    void StartGame(uint32_t now);

    void UpdatePlaying(uint32_t now);

    void ChangeState(GameState newState, uint32_t now);

    static constexpr uint8_t MAX_ENIGMAS = 4;
    static constexpr uint8_t MAX_SCREENS = 6;
    static constexpr uint32_t BLINK_MS = 250;

    Countdown &countdown;
    const Button &start;
    BombCode &code;

    Enigma *enigmas[MAX_ENIGMAS] = {};
    uint8_t enigmaCount = 0;
    Screen *screens[MAX_SCREENS] = {};
    uint8_t screenCount = 0;

    GameState state = GameState::Waiting;
    uint8_t currentEnigma = 0;
    uint32_t stateStartMs = 0;
};
