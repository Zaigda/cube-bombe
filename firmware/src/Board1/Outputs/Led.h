#pragma once
#include <cstdint>

enum class LedState : uint8_t { Off, Blinking, On };

// The status LED of an enigma station: wired to this board (StatusLed) or to the other board (RemoteLed).
class Led {
public:
    virtual ~Led() = default;

    virtual void Begin() {
    }

    virtual void Set(LedState newState) = 0;

    virtual void Update(uint32_t) {
    }
};
