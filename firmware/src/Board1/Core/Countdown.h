#pragma once
#include <cstdint>

// The bomb timer: Reset() fills it and stops it, Start() makes it count down, Stop() freezes it.
class Countdown {
public:
    void Reset(uint32_t fullMs);

    void Start(uint32_t now);

    void Stop(uint32_t now);

    uint32_t GetRemainingMs(uint32_t now) const;

    bool IsExpired(uint32_t now) const;

private:
    uint32_t startMs = 0;
    uint32_t durationMs = 0;
    bool running = false;
};
