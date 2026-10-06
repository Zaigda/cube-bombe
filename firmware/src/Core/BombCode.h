#pragma once
#include <cstdint>

class BombCode {
public:
    static constexpr int LENGTH = 4;

    void Shuffle();

    uint16_t GetColor(int index) const;

private:
    int order[LENGTH] = {0, 1, 2, 3};
};