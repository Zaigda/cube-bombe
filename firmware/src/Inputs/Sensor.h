#pragma once
#include <cstdint>

class Sensor {
public:
    virtual ~Sensor() = default;

    virtual void Begin() {
    }

    virtual void Update(uint32_t now) = 0;
};
