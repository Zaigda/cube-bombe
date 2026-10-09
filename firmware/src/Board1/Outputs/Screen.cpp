#include "Screen.h"
#include "Board1/Config.h"

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
    for (int16_t band = 0; band < height(); band += Panels::PANEL_HEIGHT) {
        DrawCenteredText(text, color, band + (Panels::PANEL_HEIGHT - textHeight) / 2);
    }
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
