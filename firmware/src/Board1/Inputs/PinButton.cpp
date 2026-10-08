#include "PinButton.h"
#include <Arduino.h>

PinButton::PinButton(uint8_t buttonPin)
    : pin(buttonPin) {
}

void PinButton::Begin() {
    pinMode(pin, INPUT_PULLUP);
    down = digitalRead(pin) == LOW;
    lastReading = down;
    lastChangeMs = millis();
}

void PinButton::Update(uint32_t now) {
    const bool reading = digitalRead(pin) == LOW;
    if (reading != lastReading) {
        lastReading = reading;
        lastChangeMs = now;
    }

    pressed = false;
    if (reading != down && now - lastChangeMs >= DEBOUNCE_MS) {
        down = reading;
        pressed = down;
    }
}

bool PinButton::IsDown() const {
    return down;
}

bool PinButton::WasPressed() const {
    return pressed;
}
