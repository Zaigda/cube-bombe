#pragma once
#include <Adafruit_GFX.h>

class Screen : public GFXcanvas16 {
public:
    Screen(uint16_t width, uint16_t height, uint8_t panelIndex);

    Screen(const Screen &) = delete;

    Screen &operator=(const Screen &) = delete;

    uint8_t GetFirstPanel() const;

    bool HasPixels() const;

private:
    uint8_t firstPanel;
};
