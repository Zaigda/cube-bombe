#pragma once
#include "DPad.h"
#include "SerialKeys.h"

class KeyboardDPad : public DPad {
public:
    KeyboardDPad(const SerialKeys &serialKeys, char up, char down, char left, char right);

    void Update(uint32_t now) override;

    Direction GetDirection() const override;

private:
    const SerialKeys &keys;
    char upKey;
    char downKey;
    char leftKey;
    char rightKey;
    Direction direction = Direction::None;
};
