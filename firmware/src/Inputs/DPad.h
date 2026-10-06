#pragma once
#include "Sensor.h"

enum class Direction : uint8_t { None, Up, Down, Left, Right };

class DPad : public Sensor {
public:
    virtual Direction GetDirection() const = 0;
};
