#include "BombCode.h"
#include <Arduino.h>
#include <utility>
#include "Core/Palette.h"

static const uint16_t COLORS[BombCode::LENGTH] = {Palette::RED, Palette::GREEN, Palette::BLUE, Palette::YELLOW};

void BombCode::Shuffle() {
    for (int i = 0; i < LENGTH; i++) {
        order[i] = i;
    }
    for (int i = LENGTH - 1; i > 0; i--) {
        const int j = random(i + 1);
        std::swap(order[i], order[j]);
    }
}

uint16_t BombCode::GetColor(int index) const {
    return COLORS[order[index]];
}
