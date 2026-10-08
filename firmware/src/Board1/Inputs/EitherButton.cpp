#include "EitherButton.h"

EitherButton::EitherButton(const Button &firstButton, const Button &secondButton)
    : first(firstButton), second(secondButton) {
}

void EitherButton::Update(uint32_t) {
}

bool EitherButton::IsDown() const {
    return first.IsDown() || second.IsDown();
}

bool EitherButton::WasPressed() const {
    return first.WasPressed() || second.WasPressed();
}
