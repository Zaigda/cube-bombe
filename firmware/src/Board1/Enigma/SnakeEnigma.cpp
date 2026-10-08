#include "SnakeEnigma.h"
#include <Arduino.h>
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

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

    length = LevelLength(0);
    targetLength = length;
    for (int i = 0; i < length; i++) {
        body[i] = {SnakeTuning::START_X / SnakeTuning::CELL_SIZE - i, SnakeTuning::START_Y / SnakeTuning::CELL_SIZE};
    }

    direction = Direction::Right;
    nextDirection = Direction::Right;
    lastStepMs = now;

    applesEaten = 0;
    lastEatMs = 0;
    PlaceFirstApple();
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
        if (applesEaten == BombCode::CABLE_COUNT) {
            return EnigmaEvent::Solved;
        }
        lastEatMs = now;
        targetLength = LevelLength(applesEaten);
        PlaceApple();
    }
    return EnigmaEvent::None;
}

void SnakeEnigma::Draw(uint32_t now) {
    const int size = SnakeTuning::CELL_SIZE;

    if (applesEaten >= BombCode::CABLE_COUNT) {
        return;
    }

    const int cable = code.GetLevelCable(applesEaten);
    screen.fillRect(apple.x * size, apple.y * size, size, size, code.GetPlugColor(code.GetPlug(cable)));

    for (int i = length - 1; i > 0; i--) {
        screen.fillRect(body[i].x * size, body[i].y * size, size, size, code.GetCableColor(cable));
    }

    int headSize = size;
    if (lastEatMs != 0 && now - lastEatMs < SnakeTuning::MOUTH_MS) {
        headSize = SnakeTuning::MOUTH_SIZE;
    }
    const int offset = (headSize - size) / 2;
    screen.fillRect(body[0].x * size - offset, body[0].y * size - offset, headSize, headSize, Palette::WHITE);
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

void SnakeEnigma::PlaceFirstApple() {
    const int size = SnakeTuning::CELL_SIZE;
    const int minColumn = SnakeTuning::FIRST_APPLE_MIN_X / size + 1;
    const int maxColumn = (SnakeTuning::FIRST_APPLE_MAX_X - 1) / size;
    const int minRow = SnakeTuning::FIRST_APPLE_MIN_Y / size + 1;
    const int maxRow = (SnakeTuning::FIRST_APPLE_MAX_Y - 1) / size;
    do {
        apple = {static_cast<int>(random(minColumn, maxColumn + 1)), static_cast<int>(random(minRow, maxRow + 1))};
    } while (IsOnSnake(apple, length));
}

int SnakeEnigma::LevelLength(int level) {
    return min(SnakeTuning::LEVEL_LENGTHS[level] / SnakeTuning::CELL_SIZE, MAX_LENGTH);
}
