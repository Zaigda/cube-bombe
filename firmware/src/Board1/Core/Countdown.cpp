#include "Countdown.h"

void Countdown::Reset(uint32_t fullMs) {
    durationMs = fullMs;
    running = false;
}

void Countdown::Start(uint32_t now) {
    if (running) {
        return;
    }
    startMs = now;
    running = true;
}

void Countdown::Stop(uint32_t now) {
    durationMs = GetRemainingMs(now);
    running = false;
}

uint32_t Countdown::GetRemainingMs(uint32_t now) const {
    if (!running) {
        return durationMs;
    }
    const uint32_t elapsedMs = now - startMs;
    return (elapsedMs >= durationMs) ? 0 : durationMs - elapsedMs;
}

bool Countdown::IsExpired(uint32_t now) const {
    return running && GetRemainingMs(now) == 0;
}

bool Countdown::IsRunning() const {
    return running;
}
