#pragma once

#include <cstdint>

namespace LinkTuning {
    constexpr uint32_t BAUD = 115200;
    constexpr uint32_t SEND_MS = 50;
    constexpr uint32_t TIMEOUT_MS = 500;
}

// The status LED of the enigma.
namespace StatusLight {
    constexpr uint32_t BLINK_MS = 400;
}