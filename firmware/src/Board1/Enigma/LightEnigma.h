#pragma once
#include "Enigma.h"
#include "Board1/Config.h"
#include "Common/Messages/Enigma3Message.h"

static_assert(LightTuning::HOLE_COUNT == LIGHT_SENSOR_COUNT, "one light value per hole");

// Enigma 3: cover the 6 holes of a secret pattern. A covered hole of the pattern lights its grid LED on board 3;
// with all 9 holes covered, every LED goes off. Each hole compares its light to the brightest it had since Start.
// Reset() takes the current light as the idle reference, so that pieces left on the plate do not press Start. While
// the enigma waits for its Start, IsAnyHoleCovered() is called every loop: it follows the idle reference and tells
// when a hole gets covered, which presses Start. IsValidMessage() rejects a broken board 3 message (a bool byte other
// than 0 or 1, or a light above MAX_LIGHT). PrintTest() and GetTestMask() serve LightTuning::TEST.
class LightEnigma : public Enigma {
public:
    explicit LightEnigma(const Enigma3InputMessage &lightSensors);

    void Reset(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

    uint16_t GetLedMask() const;

    bool IsAnyHoleCovered(uint32_t now);

    void PrintTest(uint32_t now, bool linked);

    static uint16_t GetTestMask(uint32_t now);

    static bool IsValidMessage(const Enigma3InputMessage &message);

private:
    void UpdateReferences();

    bool IsCovered(int sensor) const;

    void PrintPattern() const;

    static int GetTestLed(uint32_t now);

    const Enigma3InputMessage &sensors;

    uint16_t reference[LightTuning::HOLE_COUNT] = {};
    bool patternPrinted = false;
    int pattern = 0;
    uint16_t ledMask = 0;

    uint16_t idleReference[LightTuning::HOLE_COUNT] = {};
    uint32_t lastIdleStepMs = 0;
    uint32_t lastTestPrintMs = 0;
};
