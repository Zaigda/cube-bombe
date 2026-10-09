#pragma once
#include "Sensor.h"

// A two-state sensor: IsDown() while held, WasPressed() on one loop only for each press.
class Button : public Sensor {
public:
    virtual bool IsDown() const = 0;

    virtual bool WasPressed() const = 0;
};
