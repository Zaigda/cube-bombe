#pragma once
#include "Output.h"

class SerialOutput : public Output {
public:
    bool Begin() override;

    void Show() override;

private:
    void Print(const Screen &screen, uint8_t index) const;

    uint32_t lastFingerprints[MAX_SCREENS] = {};
};
