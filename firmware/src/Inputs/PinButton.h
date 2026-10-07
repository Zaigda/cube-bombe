#pragma once
#include "Button.h"

class PinButton : public Button {
public:
    explicit PinButton(uint8_t buttonPin);

    void Begin() override;

    void Update(uint32_t now) override;

    bool IsDown() const override;

    bool WasPressed() const override;

private:
    static constexpr uint32_t DEBOUNCE_MS = 30;

    uint8_t pin;
    bool down = false;
    bool pressed = false;
    bool lastReading = false;
    uint32_t lastChangeMs = 0;
};
