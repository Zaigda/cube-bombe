#include "CableEnigma.h"
#include "Board1/Core/Palette.h"

CableEnigma::CableEnigma(const CablePanel &cablePanel, Screen &cableScreen, const BombCode &bombCode)
    : cables(cablePanel), screen(cableScreen), code(bombCode) {
}

void CableEnigma::Start(uint32_t) {
}

EnigmaEvent CableEnigma::Update(uint32_t) {
    for (uint8_t cable = 0; cable < BombCode::CABLE_COUNT; cable++) {
        if (cables.GetPlug(cable) != code.GetPlug(cable)) {
            return EnigmaEvent::None;
        }
    }
    return EnigmaEvent::Solved;
}

void CableEnigma::Draw(uint32_t) {
    screen.DrawCenteredText("BRANCHEZ", Palette::WHITE);
}
