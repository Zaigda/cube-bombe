#pragma once
#include <cstdint>

class Countdown {
public:
    void Start(uint32_t durationMs, uint32_t now);
    void Stop(uint32_t now);              // freezes the remaining time (bomb defused)
    void AddPenalty(uint32_t penaltyMs);

    uint32_t GetRemainingMs(uint32_t now) const;
    bool IsExpired(uint32_t now) const;
    bool IsRunning() const;

private:
    uint32_t startMs = 0;      // when Start() was called
    uint32_t totalMs = 0;      // duration, minus the penalties
    uint32_t frozenMs = 0;     // remaining time shown once stopped
    bool running = false;
};