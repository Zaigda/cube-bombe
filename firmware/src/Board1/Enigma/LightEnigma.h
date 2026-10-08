#pragma once
#include "Enigma.h"
#include "Board1/Config.h"
#include "Common/LightInputMessage.h"

// Enigma 3: cover the 6 holes of a secret pattern. A covered hole of the pattern lights its LED on board 3;
// with all 9 holes covered, every LED goes off. The light of each empty hole is measured on the first Update.
class LightEnigma : public Enigma {
public:
    explicit LightEnigma(const LightInputMessage &lightSensors);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

    uint16_t GetLedMask() const;

private:
    bool IsCovered(int hole) const;

    void PrintPattern() const;

    const LightInputMessage &sensors;

    uint16_t reference[LightTuning::HOLE_COUNT] = {};
    bool calibrated = false;
    int pattern = 0;
    uint16_t ledMask = 0;
};
