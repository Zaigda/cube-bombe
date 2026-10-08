#pragma once
#include <cstdint>

// The secret of a game: which plug each cable goes into, and in which order the cables' levels are played.
class BombCode {
public:
    static constexpr int CABLE_COUNT = 3;

    void Shuffle();

    int GetLevelCable(int level) const;

    int GetPlug(int cable) const;

    uint16_t GetCableColor(int cable) const;

    uint16_t GetPlugColor(int plug) const;

private:
    int levelCable[CABLE_COUNT] = {0, 1, 2};
    int plugOfCable[CABLE_COUNT] = {0, 1, 2};
};
