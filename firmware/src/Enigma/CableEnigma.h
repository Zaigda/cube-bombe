#pragma once
#include "Enigma.h"
#include "Core/BombCode.h"
#include "Inputs/CablePanel.h"
#include "Outputs/Screen.h"

// Enigma 1, second part: shows "BRANCHEZ" and is solved once every cable is in the plug the bomb code gives it.
class CableEnigma : public Enigma {
public:
    CableEnigma(const CablePanel &cablePanel, Screen &cableScreen, const BombCode &bombCode);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

private:
    const CablePanel &cables;
    Screen &screen;
    const BombCode &code;
};
