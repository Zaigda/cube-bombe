#include "KeyboardDPad.h"

KeyboardDPad::KeyboardDPad(const SerialKeys &serialKeys, char up, char down, char left, char right)
    : keys(serialKeys), upKey(up), downKey(down), leftKey(left), rightKey(right) {
}

void KeyboardDPad::Update(uint32_t) {
    if (keys.WasTyped(upKey)) {
        direction = Direction::Up;
    } else if (keys.WasTyped(downKey)) {
        direction = Direction::Down;
    } else if (keys.WasTyped(leftKey)) {
        direction = Direction::Left;
    } else if (keys.WasTyped(rightKey)) {
        direction = Direction::Right;
    } else {
        direction = Direction::None;
    }
}

Direction KeyboardDPad::GetDirection() const {
    return direction;
}
