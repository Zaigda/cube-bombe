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

void Screen::DrawCenteredText(const char *text, uint16_t color) {
    int16_t x;
    int16_t y;
    uint16_t textWidth;
    uint16_t textHeight;
    getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
    DrawCenteredText(text, color, (height() - textHeight) / 2);
}

void Screen::DrawCenteredText(const char *text, uint16_t color, int16_t top) {
    int16_t x;
    int16_t y;
    uint16_t textWidth;
    uint16_t textHeight;
    getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
    setTextColor(color);
    setCursor((width() - textWidth) / 2, top);
    print(text);
}
