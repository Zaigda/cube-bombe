#pragma once
#include <cstdint>

// Measures AC energy, removing the microphone's DC offset (e.g. 400 at rest). A sound detected for MAX_HOLD_MS
// is a lasting noise (e.g. music started after power-on): detection is released and the background learned again.
class Microphone {
public:
    explicit Microphone(uint8_t analogPin);

    void Begin();

    void Update();

    bool IsDetected() const;

    float GetLevel() const;

private:
    void ReadSample();

    void UpdateDetection(uint32_t now);

    void FollowQuietBackground();

    void RelearnBackground();

    uint8_t pin;
    uint32_t lastSampleUs = 0;
    uint32_t lastWindowMs = 0;
    uint32_t detectedMs = 0;
    uint32_t samples = 0;
    uint32_t calibrationWindows = 0;
    float mean = 0;
    float squaredDeviation = 0;
    float background = 0;
    float level = 0;
    uint8_t attack = 0;
    uint8_t release = 0;
    bool detected = false;
};
