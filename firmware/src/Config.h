#pragma once
#include <cstdint>

namespace Timing {
    constexpr uint32_t GAME_DURATION_MS = 5 * 60 * 1000;
    constexpr uint32_t MISTAKE_PENALTY_MS = 15 * 1000;
}

namespace SnakeTuning {
    constexpr int CELL_SIZE = 4;
    constexpr uint32_t STEP_MS = 300;
    constexpr int START_LENGTH = 2;
    constexpr int START_COLUMN = 3;
    constexpr int GROWTH_FACTOR = 2;
}

namespace Debug {
    constexpr bool DIAGNOSTICS = false;
    constexpr uint32_t REPORT_EVERY_MS = 5000;
}