#pragma once
#include <cstdint>

enum class EnigmaEvent : uint8_t { None, Mistake, Solved };

// One puzzle of the bomb: Start() resets it, Update() returns None/Mistake/Solved, Draw() paints it.
class Enigma {
public:
    virtual ~Enigma() = default;

    virtual void Start(uint32_t now) = 0;

    virtual EnigmaEvent Update(uint32_t now) = 0;

    virtual void Draw(uint32_t now) = 0;
};
