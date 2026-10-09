#include "StatusLed.h"
#include "Common/Config.h"
#include <Arduino.h>

StatusLed::StatusLed(uint8_t ledPin)
    : pin(ledPin) {
}

bool StatusLed::IsLit(LedState state, uint32_t now) {
    switch (state) {
        case LedState::On:
            return true;
        case LedState::Blinking:
            return (now / StatusLight::BLINK_MS) % 2 == 0;
        case LedState::FastBlinking:
            return (now / StatusLight::FAST_BLINK_MS) % 2 == 0;
        case LedState::Off:
        default:
            return false;
    }
}

void StatusLed::Begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void StatusLed::Set(LedState newState) {
    state = newState;
}

void StatusLed::Update(uint32_t now) {
    digitalWrite(pin, IsLit(state, now) ? HIGH : LOW);
}
