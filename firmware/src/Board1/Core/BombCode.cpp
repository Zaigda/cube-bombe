#include "BombCode.h"
#include <Arduino.h>
#include <utility>
#include "Palette.h"

static const uint16_t CABLE_COLORS[BombCode::CABLE_COUNT] = {Palette::RED, Palette::GREEN, Palette::BLUE};

static const uint16_t PLUG_COLORS[BombCode::CABLE_COUNT] = {Palette::GREEN, Palette::BLUE, Palette::YELLOW};

static constexpr int SOLUTION_COUNT = 3;

static_assert(SOLUTION_COUNT >= 2, "the new code must be able to differ from the plugged cables");

static const int SOLUTIONS[SOLUTION_COUNT][BombCode::CABLE_COUNT] = {
    {0, 1, 2},
    {1, 2, 0},
    {2, 1, 0},
};

void BombCode::Shuffle() {
    const int solution = random(SOLUTION_COUNT);
    for (int cable = 0; cable < CABLE_COUNT; cable++) {
        plugOfCable[cable] = SOLUTIONS[solution][cable];
        levelCable[cable] = cable;
    }
    for (int i = CABLE_COUNT - 1; i > 0; i--) {
        const int j = random(i + 1);
        std::swap(levelCable[i], levelCable[j]);
    }

    Serial.printf("Code: levels %d %d %d, plugs %d %d %d\n", levelCable[0], levelCable[1], levelCable[2],
                  plugOfCable[0], plugOfCable[1], plugOfCable[2]);
}

int BombCode::GetLevelCable(int level) const {
    return levelCable[level];
}

int BombCode::GetPlug(int cable) const {
    return plugOfCable[cable];
}

uint16_t BombCode::GetCableColor(int cable) const {
    return CABLE_COLORS[cable];
}

uint16_t BombCode::GetPlugColor(int plug) const {
    return PLUG_COLORS[plug];
}
