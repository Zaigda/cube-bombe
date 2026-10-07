#pragma once
#include <Stream.h>
#include "Button.h"

class SerialKeyboard : public Sensor {
public:
    explicit SerialKeyboard(Stream &serial) : serial(serial) {}

    void Update(uint32_t now) override;
    const Button &Left() const { return left; }
    const Button &Right() const { return right; }
    bool TestRequested() const { return testRequested; }

private:
    class PulseButton : public Button {
    public:
        void Update(uint32_t) override {}
        bool IsDown() const override { return pressed; }
        bool WasPressed() const override { return pressed; }
        bool pressed = false;
    };

    Stream &serial;
    PulseButton left;
    PulseButton right;
    bool testRequested = false;
};
