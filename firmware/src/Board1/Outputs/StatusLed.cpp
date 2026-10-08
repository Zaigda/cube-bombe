#include "StatusLed.h"
#include <Arduino.h>
#include "Board1/Config.h"

StatusLed::StatusLed(uint8_t ledPin)
    : pin(ledPin) {
}

void StatusLed::Begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void StatusLed::Set(LedState newState) {
    state = newState;
}

void StatusLed::Update(uint32_t now) {
    bool lit = state == LedState::On;
    if (state == LedState::Blinking) {
        lit = (now / StatusLight::BLINK_MS) % 2 == 0;
    }
    digitalWrite(pin, lit ? HIGH : LOW);
}
