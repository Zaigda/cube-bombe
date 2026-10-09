#include "PinButton.h"
#include <Arduino.h>

PinButton::PinButton(uint8_t buttonPin, uint32_t maxPressMs)
    : pin(buttonPin), maxMs(maxPressMs) {
}

void PinButton::Begin() {
    pinMode(pin, INPUT_PULLUP);
    down = digitalRead(pin) == LOW;
    lastReading = down;
    lastChangeMs = millis();
    downStartMs = lastChangeMs;
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
        if (maxMs == 0) {
            pressed = down;
        } else {
            pressed = !down && now - downStartMs < maxMs;
        }
        if (down) {
            downStartMs = now;
        }
    }
}

bool PinButton::IsDown() const {
    return down;
}

bool PinButton::WasPressed() const {
    return pressed;
}
