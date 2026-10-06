#include "SerialKeys.h"
#include <Arduino.h>

void SerialKeys::Update(uint32_t) {
    keyCount = 0;
    while (Serial.available() > 0 && keyCount < MAX_KEYS) {
        keys[keyCount] = static_cast<char>(Serial.read());
        keyCount++;
    }
}

bool SerialKeys::WasTyped(char key) const {
    for (uint8_t i = 0; i < keyCount; i++) {
        if (keys[i] == key) {
            return true;
        }
    }
    return false;
}
