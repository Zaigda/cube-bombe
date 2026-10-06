#pragma once
#include "Sensor.h"

class Button : public Sensor {
public:
    virtual bool IsDown() const = 0;

    virtual bool WasPressed() const = 0;
};
