#pragma once
#include "Sensor.h"

// Keeps every sensor and calls Begin() and Update() on all of them.
class InputManager {
public:
    void Add(Sensor &sensor);

    void Begin();

    void Update(uint32_t now);

private:
    static constexpr uint8_t MAX_SENSORS = 16;

    Sensor *sensors[MAX_SENSORS] = {};
    uint8_t sensorCount = 0;
};
