#include "LightEnigma.h"
#include <Arduino.h>
#include <cstring>

LightEnigma::LightEnigma(const Enigma3InputMessage &lightSensors)
    : sensors(lightSensors) {
}

void LightEnigma::Reset(uint32_t) {
    pattern = random(LightTuning::PATTERN_COUNT);
    for (int sensor = 0; sensor < LightTuning::HOLE_COUNT; sensor++) {
        reference[sensor] = 0;
        idleReference[sensor] = sensors.light[sensor];
    }
    patternPrinted = false;
    ledMask = 0;
}

EnigmaEvent LightEnigma::Update(uint32_t) {
    if (!patternPrinted) {
        PrintPattern();
        patternPrinted = true;
    }
    UpdateReferences();

    int coveredCount = 0;
    bool matches = true;
    ledMask = 0;
    for (int sensor = 0; sensor < LightTuning::HOLE_COUNT; sensor++) {
        const int led = LightTuning::LED_OF_SENSOR[sensor];
        const bool covered = IsCovered(sensor);
        const bool inPattern = LightTuning::PATTERNS[pattern][led];
        if (covered) {
            coveredCount++;
        }
        if (covered && (inPattern || LightTuning::SHOW_ALL_COVERED)) {
            ledMask |= 1 << led;
        }
        if (covered != inPattern) {
            matches = false;
        }
    }

    if (coveredCount == LightTuning::HOLE_COUNT && !LightTuning::SHOW_ALL_COVERED) {
        ledMask = 0;
    }
    return matches ? EnigmaEvent::Solved : EnigmaEvent::None;
}

void LightEnigma::Draw(uint32_t) {
}

uint16_t LightEnigma::GetLedMask() const {
    return ledMask;
}

bool LightEnigma::IsAnyHoleCovered(uint32_t now) {
    const bool step = now - lastIdleStepMs >= LightTuning::IDLE_STEP_MS;
    if (step) {
        lastIdleStepMs = now;
    }

    bool covered = false;
    for (int sensor = 0; sensor < LightTuning::HOLE_COUNT; sensor++) {
        const uint16_t value = sensors.light[sensor];
        if (value > idleReference[sensor]) {
            idleReference[sensor] = value;
        } else if (step) {
            idleReference[sensor] -= (idleReference[sensor] - value) / LightTuning::IDLE_FOLLOW;
        }
        if (value * 100 < idleReference[sensor] * LightTuning::COVERED_PERCENT) {
            covered = true;
        }
    }
    return covered;
}

void LightEnigma::PrintTest(uint32_t now, bool linked) {
    if (!LightTuning::TEST || now - lastTestPrintMs < LightTuning::TEST_PRINT_MS) {
        return;
    }
    lastTestPrintMs = now;

    Serial.printf("Light test: board 3 %s, LED %d should be on. Sensors:\n", linked ? "linked" : "NOT LINKED",
                  GetTestLed(now));
    for (int sensor = 0; sensor < LightTuning::HOLE_COUNT; sensor++) {
        Serial.printf("  %d=%4u", sensor, sensors.light[sensor]);
        if (sensor % 3 == 2) {
            Serial.println();
        }
    }
}

uint16_t LightEnigma::GetTestMask(uint32_t now) {
    return 1 << GetTestLed(now);
}

bool LightEnigma::IsValidMessage(const Enigma3InputMessage &message) {
    uint8_t buttonByte = 0;
    memcpy(&buttonByte, &message.startButton, sizeof(buttonByte));
    if (buttonByte > 1) {
        return false;
    }
    for (uint16_t value : message.light) {
        if (value > LightTuning::MAX_LIGHT) {
            return false;
        }
    }
    return true;
}

void LightEnigma::UpdateReferences() {
    for (int sensor = 0; sensor < LightTuning::HOLE_COUNT; sensor++) {
        if (sensors.light[sensor] > reference[sensor]) {
            reference[sensor] = sensors.light[sensor];
        }
    }
}

bool LightEnigma::IsCovered(int sensor) const {
    return sensors.light[sensor] * 100 < reference[sensor] * LightTuning::COVERED_PERCENT;
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

int LightEnigma::GetTestLed(uint32_t now) {
    return (now / LightTuning::TEST_LED_MS) % LightTuning::HOLE_COUNT;
}
