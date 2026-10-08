#pragma once

#include "Led.h"

// A status LED wired to board 2: it only keeps its state, and BoardLink sends it over the wire.
class RemoteLed : public Led {
public:
    void Set(LedState newState) override;

    LedState GetState() const;

private:
    LedState state = LedState::Off;
};
