#pragma once
#include <cstdint>

enum class EnigmaEvent : uint8_t { None, Mistake, Solved };

class Enigma {
public:
    virtual ~Enigma() = default;

    virtual void Start(uint32_t now) = 0;

    virtual EnigmaEvent Update(uint32_t now) = 0;

    virtual void Draw(uint32_t now) = 0;
};