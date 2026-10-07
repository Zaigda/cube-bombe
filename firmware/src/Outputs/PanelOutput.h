#pragma once
#include "Output.h"
#include "Config.h"

class MatrixPanel_I2S_DMA;

class PanelOutput : public Output {
public:
    PanelOutput(const Pins::Hub75 &hub75Pins, uint8_t chainLength);

    bool Begin() override;

    void Show() override;

private:
    bool CheckScreen(const Screen &screen, uint8_t index) const;

    void DrawSlice(const Screen &screen, uint8_t slice);

    Pins::Hub75 pins;
    uint8_t panelCount;
    MatrixPanel_I2S_DMA *panels = nullptr;
    uint32_t lastFrameMs = 0;
};
