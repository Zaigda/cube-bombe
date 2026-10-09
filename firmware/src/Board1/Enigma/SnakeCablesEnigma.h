#pragma once
#include "Enigma.h"
#include "CableEnigma.h"
#include "SnakeEnigma.h"
#include "Board1/Core/BombCode.h"

// Enigma 1 as a whole: a new bomb code that the cables already plugged do not solve, the snake, then the cables.
// A snake crash keeps the code.
class SnakeCablesEnigma : public Enigma {
public:
    SnakeCablesEnigma(SnakeEnigma &snakePart, CableEnigma &cablesPart, BombCode &bombCode);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

private:
    SnakeEnigma &snake;
    CableEnigma &cables;
    BombCode &code;

    bool snakeDone = false;
};
