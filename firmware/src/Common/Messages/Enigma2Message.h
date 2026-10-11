#pragma once
#include <cstdint>
#include "Common/Outputs/LedState.h"

// Board 2 to board 1: its Start/Reset button and which microphone hears a sound.
struct Enigma2InputMessage {
    bool startButton;
    bool leftHeard;
    bool rightHeard;
};

// Board 1 to board 2: what the invaders status LED shows.
struct Enigma2OutputMessage {
    LedState ledState;
};
