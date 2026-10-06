#pragma once
#include "Sensor.h"

class SerialKeys : public Sensor {
public:
    void Update(uint32_t now) override;

    bool WasTyped(char key) const;

private:
    static constexpr uint8_t MAX_KEYS = 16;
    char keys[MAX_KEYS] = {};
    uint8_t keyCount = 0;
};
