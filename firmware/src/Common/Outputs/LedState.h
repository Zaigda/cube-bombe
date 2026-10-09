#pragma once
#include <cstdint>

// What a status LED shows. The numbers travel over the wire to boards 2 and 3: add new states at the end only.
enum class LedState : uint8_t { Off = 0, Blinking = 1, On = 2, FastBlinking = 3 };
