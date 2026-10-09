#pragma once
#include "Enigma.h"
#include "Board1/Core/BombCode.h"
#include "Board1/Outputs/Screen.h"
#include "Common/Inputs/CablePanel.h"

// Enigma 1, second part: shows Texts::PLUG and is solved once every cable is in the plug the bomb code gives it.
class CableEnigma : public Enigma {
public:
    CableEnigma(const CablePanel &cablePanel, Screen &cableScreen, const BombCode &bombCode);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

    bool IsSolved() const;

private:
    const CablePanel &cables;
    Screen &screen;
    const BombCode &code;
};
