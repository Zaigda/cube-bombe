#pragma once
#include "Board1/Config.h"
#include "Board1/Outputs/Screen.h"

class MatrixPanel_I2S_DMA;

// Shows Screens on the HUB75 panel chain: AddScreen() then Begin() in setup, then Show() every loop. Each panel-high
// slice of a Screen goes to one panel, flipped if ROTATED_180.
class PanelOutput {
public:
    PanelOutput(const Pins::Hub75 &hub75Pins, uint8_t chainLength);

    PanelOutput(const PanelOutput &) = delete;

    PanelOutput &operator=(const PanelOutput &) = delete;

    void AddScreen(Screen &screen);

    bool Begin();

    void Show();

    const char *GetError() const;

private:
    bool CheckScreen(const Screen &screen, uint8_t index, bool panelUsed[]);

    void DrawSlice(const Screen &screen, uint8_t slice);

    static constexpr uint8_t MAX_SCREENS = 6;

    Pins::Hub75 pins;
    uint8_t panelCount;
    Screen *screens[MAX_SCREENS] = {};
    uint8_t screenCount = 0;
    MatrixPanel_I2S_DMA *panels = nullptr;
    uint32_t lastFrameMs = 0;
    char error[80] = "";
};
