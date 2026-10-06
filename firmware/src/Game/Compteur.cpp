//
// Created by Paul Hugy on 06/10/2026.
//

#include "Compteur.h"

void Compteur::Start(uint32_t duration, uint32_t now) {
    durationMs = duration;
    startMs = now;
    running = true;
}

void Compteur::Penalty(uint32_t ms) {
    if (!running) {
        return;
    }
    durationMs = (ms >= durationMs) ? 0 : durationMs - ms;

}

uint32_t Compteur::RemainingTime(uint32_t now) const {
    if (!running) {
        return 0;
    }

    const uint32_t elapsed = now - startMs;
    return (elapsed >= durationMs) ? 0 : durationMs - elapsed;

}

bool Compteur::IsExpired(uint32_t now) const {
    return running && RemainingTime(now) == 0;

}
