#pragma once
#include "Common/Outputs/Output.h"
#include "Board1/Config.h"

class MatrixPanel_I2S_DMA;

// Shows Screens on the HUB75 panel chain: each panel-high slice goes to one panel, flipped if ROTATED_180.
class PanelOutput : public Output {
public:
    PanelOutput(const Pins::Hub75 &hub75Pins, uint8_t chainLength);

    PanelOutput(const PanelOutput &) = delete;

    PanelOutput &operator=(const PanelOutput &) = delete;

    bool Begin() override;

    void Show() override;

private:
    bool CheckScreen(const Screen &screen, uint8_t index, bool panelUsed[]) const;

    void DrawSlice(const Screen &screen, uint8_t slice);

    Pins::Hub75 pins;
    uint8_t panelCount;
    MatrixPanel_I2S_DMA *panels = nullptr;
    uint32_t lastFrameMs = 0;
};
