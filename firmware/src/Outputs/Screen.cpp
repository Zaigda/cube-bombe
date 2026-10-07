#include "Screen.h"

Screen::Screen(uint16_t width, uint16_t height, uint8_t panelIndex)
    : GFXcanvas16(width, height), firstPanel(panelIndex) {
}

uint8_t Screen::GetFirstPanel() const {
    return firstPanel;
}

bool Screen::HasPixels() const {
    return getBuffer() != nullptr;
}
