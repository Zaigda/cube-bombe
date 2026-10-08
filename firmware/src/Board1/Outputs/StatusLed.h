#pragma once
#include <cstdint>

enum class LedState : uint8_t { Off, Blinking, On };

// A single LED between a pin and GND (through a resistor): Off, Blinking (BLINK_MS) or On.
class StatusLed {
public:
    explicit StatusLed(uint8_t ledPin);

    void Begin();

    void Set(LedState newState);

    void Update(uint32_t now);

private:
    uint8_t pin;
    LedState state = LedState::Off;
};
