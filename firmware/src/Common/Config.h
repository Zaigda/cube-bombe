#pragma once
#include <cstddef>
#include <cstdint>

// The serial wires between board 1 and boards 2 and 3.
namespace LinkTuning {
    constexpr uint32_t BAUD = 115200;
    constexpr uint32_t SEND_MS = 50;
    constexpr uint32_t TIMEOUT_MS = 500;
}

// The status LED of the enigma: Blinking while it is played, FastBlinking when the bomb explodes.
namespace StatusLight {
    constexpr uint32_t BLINK_MS = 400;
    constexpr uint32_t FAST_BLINK_MS = 150;
}

// Pin checks for the Config.h of each board: used in static_assert, so a wrong pin choice does not build.
namespace PinCheck {
    // Native USB (19, 20), flash and PSRAM (26 to 37) and the COM port (43, 44): never wire anything to them.
    // 22 to 25 and above 48 do not exist on the ESP32-S3.
    constexpr bool IsReservedPin(int pin) {
        return pin == 19 || pin == 20 || (pin >= 22 && pin <= 37) || pin == 43 || pin == 44 || pin > 48;
    }

    constexpr bool HasReservedPin(const uint8_t *pins, size_t count) {
        for (size_t i = 0; i < count; i++) {
            if (IsReservedPin(pins[i])) {
                return true;
            }
        }
        return false;
    }

    constexpr bool HasDuplicatePin(const uint8_t *pins, size_t count) {
        for (size_t i = 0; i < count; i++) {
            for (size_t j = i + 1; j < count; j++) {
                if (pins[i] == pins[j]) {
                    return true;
                }
            }
        }
        return false;
    }
}
