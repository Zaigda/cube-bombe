#pragma once
#include "Button.h"
#include "SerialKeys.h"

class KeyButton : public Button {
public:
    KeyButton(const SerialKeys &serialKeys, char triggerKey);

    void Update(uint32_t now) override;

    bool IsDown() const override;

    bool WasPressed() const override;

private:
    const SerialKeys &keys;
    char key;
    bool pressed = false;
};
