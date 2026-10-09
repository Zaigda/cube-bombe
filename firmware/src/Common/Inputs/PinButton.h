#pragma once
#include "Button.h"

// A push button between a pin and GND, using the internal pull-up, debounced (DEBOUNCE_MS). With a maxPressMs,
// WasPressed() comes on release and only after a press shorter than maxPressMs: a long press is left to IsDown().
class PinButton : public Button {
public:
    explicit PinButton(uint8_t buttonPin, uint32_t maxPressMs = 0);

    void Begin() override;

    void Update(uint32_t now) override;

    bool IsDown() const override;

    bool WasPressed() const override;

private:
    static constexpr uint32_t DEBOUNCE_MS = 30;

    uint8_t pin;
    uint32_t maxMs;
    bool down = false;
    bool pressed = false;
    bool lastReading = false;
    uint32_t lastChangeMs = 0;
    uint32_t downStartMs = 0;
};
