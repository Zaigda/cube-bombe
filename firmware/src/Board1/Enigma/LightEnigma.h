#pragma once
#include "Enigma.h"
#include "Board1/Config.h"
#include "Common/Messages/Enigma3Message.h"

// Enigma 3: cover the 6 holes of a secret pattern. A covered hole of the pattern lights its LED on board 3;
// with all 9 holes covered, every LED goes off. Each hole compares its light to the brightest it had since Start.
class LightEnigma : public Enigma {
public:
    explicit LightEnigma(const Enigma3InputMessage &lightSensors);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

    uint16_t GetLedMask() const;

private:
    void UpdateReferences();

    bool IsCovered(int hole) const;

    void PrintPattern() const;

    const Enigma3InputMessage &sensors;

    uint16_t reference[LightTuning::HOLE_COUNT] = {};
    bool patternPrinted = false;
    int pattern = 0;
    uint16_t ledMask = 0;
};
