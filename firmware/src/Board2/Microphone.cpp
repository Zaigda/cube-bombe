#include "Microphone.h"
#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include "Board2/Config.h"

Microphone::Microphone(uint8_t analogPin)
    : pin(analogPin) {
}

void Microphone::Begin() {
    *this = Microphone(pin);
    pinMode(pin, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(pin, ADC_11db);
    lastSampleUs = micros();
    lastWindowMs = millis();
}

void Microphone::Update() {
    ReadSample();
    const uint32_t now = millis();
    if (now - lastWindowMs < SoundTuning::WINDOW_MS) {
        return;
    }
    lastWindowMs = now;
    if (samples < 2) {
        return;
    }
    level = std::sqrt(std::max(0.0f, squaredDeviation / samples));
    samples = 0;
    mean = 0;
    squaredDeviation = 0;
    UpdateDetection(now);
}

bool Microphone::IsDetected() const {
    return detected;
}

float Microphone::GetLevel() const {
    return level;
}

void Microphone::ReadSample() {
    const uint32_t nowUs = micros();
    if (nowUs - lastSampleUs < SoundTuning::SAMPLE_US) {
        return;
    }
    lastSampleUs = nowUs;
    const float value = analogRead(pin);
    const float delta = value - mean;
    samples++;
    mean += delta / samples;
    squaredDeviation += delta * (value - mean);
}

void Microphone::UpdateDetection(uint32_t now) {
    if (calibrationWindows < SoundTuning::CALIBRATION_WINDOWS) {
        calibrationWindows++;
        background += (level - background) / calibrationWindows;
        return;
    }
    if (detected && now - detectedMs >= SoundTuning::MAX_HOLD_MS) {
        RelearnBackground();
        return;
    }

    const float threshold = std::max(background * SoundTuning::NOISE_RATIO, background + SoundTuning::MIN_MARGIN);
    if (!detected) {
        attack = level >= threshold ? attack + 1 : 0;
        if (attack >= SoundTuning::ATTACK_WINDOWS) {
            detected = true;
            detectedMs = now;
            release = 0;
        }
    } else {
        const float releaseThreshold = background + (threshold - background) * SoundTuning::RELEASE_RATIO;
        release = level < releaseThreshold ? release + 1 : 0;
        if (release >= SoundTuning::RELEASE_WINDOWS) {
            detected = false;
            attack = 0;
        }
    }
    FollowQuietBackground();
}

void Microphone::FollowQuietBackground() {
    if (!detected && attack == 0) {
        background += SoundTuning::BACKGROUND_FOLLOW * (level - background);
    }
}

void Microphone::RelearnBackground() {
    detected = false;
    attack = 0;
    release = 0;
    calibrationWindows = 0;
}
