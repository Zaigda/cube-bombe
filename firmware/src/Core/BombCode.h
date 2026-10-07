#pragma once
#include <cstdint>

// The secret color order of a game. Shuffle() picks a new random order.
class BombCode {
public:
    static constexpr int LENGTH = 4;

    void Shuffle();

    uint16_t GetColor(int index) const;

private:
    int order[LENGTH] = {0, 1, 2, 3};
};
