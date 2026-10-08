#include "LightEnigma.h"
#include <Arduino.h>

LightEnigma::LightEnigma(const Enigma3InputMessage &lightSensors)
    : sensors(lightSensors) {
}

void LightEnigma::Start(uint32_t) {
    pattern = random(LightTuning::PATTERN_COUNT);
    calibrated = false;
    ledMask = 0;
}

EnigmaEvent LightEnigma::Update(uint32_t) {
    if (!calibrated) {
        for (int hole = 0; hole < LightTuning::HOLE_COUNT; hole++) {
            reference[hole] = sensors.light[hole];
        }
        calibrated = true;
        PrintPattern();
    }

    int coveredCount = 0;
    bool matches = true;
    ledMask = 0;
    for (int hole = 0; hole < LightTuning::HOLE_COUNT; hole++) {
        const bool covered = IsCovered(hole);
        const bool inPattern = LightTuning::PATTERNS[pattern][hole];
        if (covered) {
            coveredCount++;
        }
        if (covered && inPattern) {
            ledMask |= 1 << hole;
        }
        if (covered != inPattern) {
            matches = false;
        }
    }

    if (coveredCount == LightTuning::HOLE_COUNT) {
        ledMask = 0;
    }
    return matches ? EnigmaEvent::Solved : EnigmaEvent::None;
}

void LightEnigma::Draw(uint32_t) {
}

uint16_t LightEnigma::GetLedMask() const {
    return ledMask;
}

bool LightEnigma::IsCovered(int hole) const {
    return sensors.light[hole] * 100 < reference[hole] * LightTuning::COVERED_PERCENT;
}

void LightEnigma::PrintPattern() const {
    Serial.printf("Light: pattern %d, cover the X holes\n", pattern);
    for (int row = 0; row < 3; row++) {
        Serial.print("    ");
        for (int col = 0; col < 3; col++) {
            Serial.print(LightTuning::PATTERNS[pattern][row * 3 + col] ? " X" : " .");
        }
        Serial.println();
    }
}
