#include "Countdown.h"

void Countdown::Start(uint32_t durationMs, uint32_t now) {
    startMs = now;
    totalMs = durationMs;
    running = true;
}

void Countdown::Stop(uint32_t now) {
    frozenMs = GetRemainingMs(now);
    running = false;
}

void Countdown::AddPenalty(uint32_t penaltyMs) {
    if (!running) {
        return;
    }
    totalMs = (penaltyMs >= totalMs) ? 0 : totalMs - penaltyMs;
}

uint32_t Countdown::GetRemainingMs(uint32_t now) const {
    if (!running) {
        return frozenMs;
    }
    const uint32_t elapsedMs = now - startMs;
    return (elapsedMs >= totalMs) ? 0 : totalMs - elapsedMs;
}

bool Countdown::IsExpired(uint32_t now) const {
    return running && GetRemainingMs(now) == 0;
}

bool Countdown::IsRunning() const {
    return running;
}