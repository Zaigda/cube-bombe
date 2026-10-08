#include "RemoteLed.h"

void RemoteLed::Set(LedState newState) {
    state = newState;
}

LedState RemoteLed::GetState() const {
    return state;
}
