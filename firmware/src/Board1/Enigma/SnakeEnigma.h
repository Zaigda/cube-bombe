#pragma once
#include "Enigma.h"
#include "Board1/Config.h"
#include "Board1/Core/BombCode.h"
#include "Board1/Outputs/Screen.h"
#include "Common/Inputs/Button.h"

// Where the snake's head goes, on the screen.
enum class Direction : uint8_t { Up, Down, Left, Right };

// A quarter turn to the snake's own left or right, waiting in the queue.
enum class Turn : uint8_t { Left, Right };

// One square of the snake grid; x and y count cells, not LEDs.
struct Cell {
    int x;
    int y;
};

// Enigma 1: a two-button snake plays one level per cable (snake = cable color, apple = its plug's color);
// a crash restarts it at level 1 with the same code. Turn presses wait in a queue, one is used per step.
class SnakeEnigma : public Enigma {
public:
    SnakeEnigma(const Button &leftButton, const Button &rightButton, Screen &snakeScreen, const BombCode &bombCode);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

private:
    void ReadTurns();

    void QueueTurn(Turn turn);

    void ApplyNextTurn();

    Cell NextHead() const;

    bool IsOutside(Cell cell) const;

    bool IsOnSnake(Cell cell, int cellsToCheck) const;

    void Move(Cell newHead);

    void PlaceApple();

    void PlaceFirstApple();

    static int LevelLength(int level);

    static constexpr int MAX_LENGTH = 64;

    const Button &turnLeftButton;
    const Button &turnRightButton;
    Screen &screen;
    const BombCode &code;

    int columns = 0;
    int rows = 0;
    Cell body[MAX_LENGTH] = {};
    int length = 0;
    int targetLength = 0;
    Direction direction = Direction::Right;
    Turn queuedTurns[SnakeTuning::TURN_QUEUE_SIZE] = {};
    int queuedTurnCount = 0;
    uint32_t lastStepMs = 0;
    Cell apple = {0, 0};
    int applesEaten = 0;
    uint32_t lastEatMs = 0;
};
