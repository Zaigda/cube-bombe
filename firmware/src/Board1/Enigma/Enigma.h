#pragma once
#include <cstdint>

// What an enigma reports after each Update(): nothing, a mistake ("RATE"), or solved.
enum class EnigmaEvent : uint8_t { None, Mistake, Solved };

// One puzzle of the bomb: Reset() puts it back to its beginning, Update() returns None/Mistake/Solved and Draw()
// paints it. The player's Start does not call Reset(): it only lets EnigmaStation call Update() and Draw().
class Enigma {
public:
    virtual ~Enigma() = default;

    virtual void Reset(uint32_t now) = 0;

    virtual EnigmaEvent Update(uint32_t now) = 0;

    virtual void Draw(uint32_t now) = 0;
};
