#pragma once
#include "Button.h"

// Two buttons seen as one: down if either is down. Add both real buttons to the InputManager, not this one.
class EitherButton : public Button {
public:
    EitherButton(const Button &firstButton, const Button &secondButton);

    void Update(uint32_t now) override;

    bool IsDown() const override;

    bool WasPressed() const override;

private:
    const Button &first;
    const Button &second;
};
