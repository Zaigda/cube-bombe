#include "RemoteButton.h"

void RemoteButton::SetDown(bool isDown) {
    down = isDown;
}

void RemoteButton::Update(uint32_t) {
    pressed = down && !wasDown;
    wasDown = down;
}

bool RemoteButton::IsDown() const {
    return down;
}

bool RemoteButton::WasPressed() const {
    return pressed;
}
