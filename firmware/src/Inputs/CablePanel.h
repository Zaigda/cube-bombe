#pragma once
#include "Sensor.h"

// The 3 cables and 3 plugs of enigma 1: one plug at a time goes LOW, and a cable (INPUT_PULLUP) that reads LOW
// is in it. GetPlug() gives the plug of a cable, or UNPLUGGED. Prints one monitor line per change.
class CablePanel : public Sensor {
public:
    static constexpr uint8_t COUNT = 3;
    static constexpr int8_t UNPLUGGED = -1;

    CablePanel(const uint8_t (&cablePins)[COUNT], const uint8_t (&plugPins)[COUNT]);

    void Begin() override;

    void Update(uint32_t now) override;

    int8_t GetPlug(uint8_t cable) const;

private:
    int8_t ReadPlug(uint8_t cable) const;

    static constexpr uint32_t SETTLE_US = 10;
    static constexpr uint32_t DEBOUNCE_MS = 30;

    uint8_t cablePin[COUNT] = {};
    uint8_t plugPin[COUNT] = {};
    int8_t plug[COUNT] = {UNPLUGGED, UNPLUGGED, UNPLUGGED};
    int8_t lastReading[COUNT] = {UNPLUGGED, UNPLUGGED, UNPLUGGED};
    uint32_t lastChangeMs[COUNT] = {};
};
