#pragma once
#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "Config.h"

// Measures AC energy, removing the microphone's DC offset (e.g. 400 at rest). A sound detected for MAX_HOLD_MS
// is a lasting noise (e.g. music started after power-on): detection is released and the background learned again.
class Microphone {
public:
    explicit Microphone(uint8_t analogPin) : pin(analogPin) {}

    void Begin() {
        *this = Microphone(pin);
        pinMode(pin, INPUT);
        analogReadResolution(12);
        analogSetPinAttenuation(pin, ADC_11db);
        lastSampleUs = micros();
        lastWindowMs = millis();
    }

    bool Detected() const { return detected; }
    float Level() const { return level; }

    void Update() {
        const uint32_t nowUs = micros();
        if (nowUs - lastSampleUs >= SoundTuning::SAMPLE_US) {
            lastSampleUs = nowUs;
            const float value = analogRead(pin);
            const float delta = value - mean;
            mean += delta / ++samples;
            squaredDeviation += delta * (value - mean);
        }
        const uint32_t now = millis();
        if (now - lastWindowMs < SoundTuning::WINDOW_MS) return;
        lastWindowMs = now;
        if (samples < 2) return;
        level = std::sqrt(std::max(0.0f, squaredDeviation / samples));
        samples = 0;
        mean = squaredDeviation = 0;

        if (calibrationWindows < SoundTuning::CALIBRATION_WINDOWS) {
            ++calibrationWindows;
            background += (level - background) / calibrationWindows;
            return;
        }

        if (detected && now - detectedMs >= SoundTuning::MAX_HOLD_MS) {
            RelearnBackground();
            return;
        }

        const float threshold = std::max(background * SoundTuning::NOISE_RATIO,
                                         background + SoundTuning::MIN_MARGIN);
        if (!detected) {
            attack = level >= threshold ? attack + 1 : 0;
            if (attack >= SoundTuning::ATTACK_WINDOWS) {
                detected = true;
                detectedMs = now;
                release = 0;
            }
        } else {
            const float releaseThreshold = background + (threshold - background) * 0.6f;
            release = level < releaseThreshold ? release + 1 : 0;
            if (release >= SoundTuning::RELEASE_WINDOWS) {
                detected = false;
                attack = 0;
            }
        }

        FollowQuietBackground();
    }

private:
    void FollowQuietBackground() {
        if (!detected && attack == 0) {
            background += 0.02f * (level - background);
        }
    }

    void RelearnBackground() {
        detected = false;
        attack = 0;
        release = 0;
        calibrationWindows = 0;
    }

    uint8_t pin;
    uint32_t lastSampleUs = 0, lastWindowMs = 0, detectedMs = 0;
    uint32_t samples = 0, calibrationWindows = 0;
    float mean = 0, squaredDeviation = 0, background = 0, level = 0;
    uint8_t attack = 0, release = 0;
    bool detected = false;
};
