//
// Created by Paul Hugy on 06/10/2026.
//

#ifndef FIRMWARE_COMPTEUR_H
#define FIRMWARE_COMPTEUR_H
#include <cstdint>


class Compteur {
public:
    void Start(uint32_t duration, uint32_t now);
    void Penalty(uint32_t ms);
    uint32_t RemainingTime(uint32_t now) const;
    bool IsExpired(uint32_t now) const;

private:
    uint32_t startMs{0};
    uint32_t durationMs{0};
    bool running{false};
};


#endif //FIRMWARE_COMPTEUR_H
