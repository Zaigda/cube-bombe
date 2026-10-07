#include "SnakeEnigma.h"
#include <Arduino.h>
#include "Config.h"
#include "Core/Palette.h"

static Direction TurnLeft(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Left;
        case Direction::Left:
            return Direction::Down;
        case Direction::Down:
            return Direction::Right;
        case Direction::Right:
        default:
            return Direction::Up;
    }
}

static Direction TurnRight(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Right;
        case Direction::Right:
            return Direction::Down;
        case Direction::Down:
            return Direction::Left;
        case Direction::Left:
        default:
            return Direction::Up;
    }
}

static bool SameCell(Cell a, Cell b) {
    return a.x == b.x && a.y == b.y;
}

SnakeEnigma::SnakeEnigma(const Button &leftButton, const Button &rightButton, Screen &snakeScreen,
                         const BombCode &bombCode)
    : turnLeftButton(leftButton), turnRightButton(rightButton), screen(snakeScreen), code(bombCode) {
}

void SnakeEnigma::Start(uint32_t now) {
    columns = screen.width() / SnakeTuning::CELL_SIZE;
    rows = screen.height() / SnakeTuning::CELL_SIZE;

    length = SnakeTuning::START_LENGTH;
    targetLength = SnakeTuning::START_LENGTH;
    for (int i = 0; i < length; i++) {
        body[i] = {SnakeTuning::START_COLUMN - i, rows / 2};
    }

    direction = Direction::Right;
    nextDirection = Direction::Right;
    lastStepMs = now;

    applesEaten = 0;
    PlaceApple();
}

EnigmaEvent SnakeEnigma::Update(uint32_t now) {
    ReadTurns();

    if (now - lastStepMs < SnakeTuning::STEP_MS) {
        return EnigmaEvent::None;
    }
    lastStepMs = now;
    direction = nextDirection;

    const Cell next = NextHead();
    const bool growing = length < targetLength;
    const int cellsThatStay = growing ? length : length - 1;
    if (IsOutside(next) || IsOnSnake(next, cellsThatStay)) {
        Start(now);
        return EnigmaEvent::Mistake;
    }

    Move(next);

    if (SameCell(next, apple)) {
        applesEaten++;
        if (applesEaten == BombCode::LENGTH) {
            return EnigmaEvent::Solved;
        }
        targetLength = min(targetLength * SnakeTuning::GROWTH_FACTOR, MAX_LENGTH);
        PlaceApple();
    }
    return EnigmaEvent::None;
}

void SnakeEnigma::Draw(uint32_t) {
    const int size = SnakeTuning::CELL_SIZE;

    if (applesEaten < BombCode::LENGTH) {
        screen.fillRect(apple.x * size, apple.y * size, size, size, code.GetColor(applesEaten));
    }

    for (int i = 0; i < length; i++) {
        const uint16_t color = (i == 0) ? Palette::ORANGE : Palette::WHITE;
        screen.fillRect(body[i].x * size, body[i].y * size, size, size, color);
    }
}

void SnakeEnigma::ReadTurns() {
    if (turnLeftButton.WasPressed()) {
        nextDirection = TurnLeft(direction);
    }
    if (turnRightButton.WasPressed()) {
        nextDirection = TurnRight(direction);
    }
}

Cell SnakeEnigma::NextHead() const {
    Cell next = body[0];
    switch (direction) {
        case Direction::Up:
            next.y--;
            break;
        case Direction::Down:
            next.y++;
            break;
        case Direction::Left:
            next.x--;
            break;
        case Direction::Right:
            next.x++;
            break;
    }
    return next;
}

bool SnakeEnigma::IsOutside(Cell cell) const {
    return cell.x < 0 || cell.x >= columns || cell.y < 0 || cell.y >= rows;
}

bool SnakeEnigma::IsOnSnake(Cell cell, int cellsToCheck) const {
    for (int i = 0; i < cellsToCheck; i++) {
        if (SameCell(body[i], cell)) {
            return true;
        }
    }
    return false;
}

void SnakeEnigma::Move(Cell newHead) {
    if (length < targetLength) {
        length++;
    }
    for (int i = length - 1; i > 0; i--) {
        body[i] = body[i - 1];
    }
    body[0] = newHead;
}

void SnakeEnigma::PlaceApple() {
    do {
        apple = {static_cast<int>(random(columns)), static_cast<int>(random(rows))};
    } while (IsOnSnake(apple, length));
}
