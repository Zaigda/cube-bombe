#pragma once
#include "Sensor.h"

// A two-state sensor: IsDown() while held, WasPressed() only on the loop where the press starts.
class Button : public Sensor {
public:
    virtual bool IsDown() const = 0;

    virtual bool WasPressed() const = 0;
};
