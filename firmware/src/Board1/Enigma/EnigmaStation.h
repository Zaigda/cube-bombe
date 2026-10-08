#pragma once
#include "Enigma.h"
#include "Board1/Inputs/Button.h"
#include "Board1/Outputs/Screen.h"
#include "Board1/Outputs/Led.h"

// The box around one enigma: its Start/Reset button, its status LED, and the screens shown around the game
// (paused with "START", "RATE" after a mistake, "DESAMORCE" once solved). textScreen can be nullptr.
class EnigmaStation {
public:
    EnigmaStation(const char *stationName, Enigma &stationEnigma, const Button &startButton, Led &statusLed,
                  Screen *textScreen);

    void Begin(uint32_t now);

    void Update(uint32_t now);

    void Draw(uint32_t now);

    bool IsPlaying() const;

private:
    enum class Phase : uint8_t { Paused, Playing, Failed, Solved };

    void Reset(uint32_t now);

    const char *name;
    Enigma &enigma;
    const Button &start;
    Led &led;
    Screen *screen;

    Phase phase = Phase::Paused;
    uint32_t failedMs = 0;
};
