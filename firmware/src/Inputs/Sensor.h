#pragma once
#include <cstdint>

// Anything InputManager reads: Begin() once in setup, Update() every loop.
class Sensor {
public:
    virtual ~Sensor() = default;

    virtual void Begin() {
    }

    virtual void Update(uint32_t now) = 0;
};
