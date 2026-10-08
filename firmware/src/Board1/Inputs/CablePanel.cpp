#include "CablePanel.h"
#include <Arduino.h>

CablePanel::CablePanel(const uint8_t (&cablePins)[COUNT], const uint8_t (&plugPins)[COUNT]) {
    for (uint8_t i = 0; i < COUNT; i++) {
        cablePin[i] = cablePins[i];
        plugPin[i] = plugPins[i];
    }
}

void CablePanel::Begin() {
    for (uint8_t i = 0; i < COUNT; i++) {
        pinMode(cablePin[i], INPUT_PULLUP);
        pinMode(plugPin[i], INPUT);
    }
}

void CablePanel::Update(uint32_t now) {
    for (uint8_t cable = 0; cable < COUNT; cable++) {
        const int8_t reading = ReadPlug(cable);
        if (reading != lastReading[cable]) {
            lastReading[cable] = reading;
            lastChangeMs[cable] = now;
        }

        if (reading != plug[cable] && now - lastChangeMs[cable] >= DEBOUNCE_MS) {
            plug[cable] = reading;
            Serial.printf("Cable %d -> plug %d\n", cable, reading);
        }
    }
}

int8_t CablePanel::GetPlug(uint8_t cable) const {
    return plug[cable];
}

int8_t CablePanel::ReadPlug(uint8_t cable) const {
    int8_t found = UNPLUGGED;
    for (uint8_t p = 0; p < COUNT && found == UNPLUGGED; p++) {
        pinMode(plugPin[p], OUTPUT);
        digitalWrite(plugPin[p], LOW);
        delayMicroseconds(SETTLE_US);
        if (digitalRead(cablePin[cable]) == LOW) {
            found = static_cast<int8_t>(p);
        }
        pinMode(plugPin[p], INPUT);
    }
    return found;
}
