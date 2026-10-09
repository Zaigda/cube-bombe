#include "CableEnigma.h"
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

CableEnigma::CableEnigma(const CablePanel &cablePanel, Screen &cableScreen, const BombCode &bombCode)
    : cables(cablePanel), screen(cableScreen), code(bombCode) {
}

void CableEnigma::Start(uint32_t) {
}

EnigmaEvent CableEnigma::Update(uint32_t) {
    return IsSolved() ? EnigmaEvent::Solved : EnigmaEvent::None;
}

void CableEnigma::Draw(uint32_t) {
    screen.DrawCenteredText(Texts::PLUG, Palette::WHITE);
}

bool CableEnigma::IsSolved() const {
    for (uint8_t cable = 0; cable < BombCode::CABLE_COUNT; cable++) {
        if (cables.GetPlug(cable) != code.GetPlug(cable)) {
            return false;
        }
    }
    return true;
}
