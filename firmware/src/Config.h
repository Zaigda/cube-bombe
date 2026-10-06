#pragma once
#include <cstdint>

// Every value the game designer may want to tweak lives in this file.
namespace Timing {
    constexpr uint32_t GAME_DURATION_MS = 5UL * 60UL * 1000UL;   // 5 minutes
    constexpr uint32_t MISTAKE_PENALTY_MS = 15UL * 1000UL;       // 15 seconds
}