#pragma once

#include "Led.h"

// A single LED between a pin and GND (through a resistor): Off, Blinking (BLINK_MS) or On.
class StatusLed : public Led {
public:
    explicit StatusLed(uint8_t ledPin);

    void Begin() override;

    void Set(LedState newState) override;

    void Update(uint32_t now) override;

private:
    uint8_t pin;
    LedState state = LedState::Off;
};
