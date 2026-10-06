#include "KeyButton.h"

KeyButton::KeyButton(const SerialKeys &serialKeys, char triggerKey)
    : keys(serialKeys), key(triggerKey) {
}

void KeyButton::Update(uint32_t) {
    pressed = keys.WasTyped(key);
}

bool KeyButton::IsDown() const {
    return pressed;
}

bool KeyButton::WasPressed() const {
    return pressed;
}
