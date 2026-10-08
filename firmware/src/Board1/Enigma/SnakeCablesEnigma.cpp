#include "SnakeCablesEnigma.h"
#include <Arduino.h>

SnakeCablesEnigma::SnakeCablesEnigma(SnakeEnigma &snakePart, CableEnigma &cablesPart, BombCode &bombCode)
    : snake(snakePart), cables(cablesPart), code(bombCode) {
}

void SnakeCablesEnigma::Start(uint32_t now) {
    code.Shuffle();
    snake.Start(now);
    snakeDone = false;
}

EnigmaEvent SnakeCablesEnigma::Update(uint32_t now) {
    if (snakeDone) {
        return cables.Update(now);
    }

    const EnigmaEvent event = snake.Update(now);
    if (event == EnigmaEvent::Solved) {
        Serial.println("Snake done: plug the cables");
        cables.Start(now);
        snakeDone = true;
        return EnigmaEvent::None;
    }
    return event;
}

void SnakeCablesEnigma::Draw(uint32_t now) {
    if (snakeDone) {
        cables.Draw(now);
    } else {
        snake.Draw(now);
    }
}
