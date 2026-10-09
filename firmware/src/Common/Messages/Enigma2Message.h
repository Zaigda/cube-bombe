#pragma once
#include <Arduino.h>
#include "Common/Outputs/LedState.h"

// Board 2 to board 1: its Start/Reset button and which microphone hears a sound.
struct Enigma2InputMessage {
    bool resetBtn;
    bool leftBtn;
    bool rightBtn;
};

// Board 1 to board 2: what the invaders status LED shows.
struct Enigma2OutputMessage {
    LedState ledState;
};
