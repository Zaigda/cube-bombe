#pragma once
#include "Button.h"

// A button wired to board 2: BoardLink calls SetDown() with what board 2 sends.
class RemoteButton : public Button {
public:
    void SetDown(bool isDown);

    void Update(uint32_t now) override;

    bool IsDown() const override;

    bool WasPressed() const override;

private:
    bool down = false;
    bool wasDown = false;
    bool pressed = false;
};
