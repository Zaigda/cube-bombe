#pragma once
#include "LedState.h"

// A single LED between a pin and GND (through a resistor) that shows a LedState. IsLit() is the blinking rule.
class StatusLed {
public:
    explicit StatusLed(uint8_t ledPin);

    static bool IsLit(LedState state, uint32_t now);

    void Begin();

    void Set(LedState newState);

    void Update(uint32_t now);

private:
    uint8_t pin;
    LedState state = LedState::Off;
};
