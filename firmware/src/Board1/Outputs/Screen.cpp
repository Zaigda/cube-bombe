#include "Screen.h"
#include <cstdio>
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
    for (int16_t band = 0; band < height(); band += Panels::PANEL_HEIGHT) {
        DrawCenteredOnPanel(text, color, band);
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

void Screen::DrawCenteredOnPanel(const char *text, uint16_t color, int16_t panelTop) {
    int16_t x;
    int16_t y;
    uint16_t textWidth;
    uint16_t textHeight;
    getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
    DrawCenteredText(text, color, panelTop + (Panels::PANEL_HEIGHT - textHeight) / 2);
}

void Screen::DrawCenteredTime(uint32_t ms, uint16_t color, int16_t top) {
    const unsigned totalSeconds = (ms + 999) / 1000;
    char text[12];
    snprintf(text, sizeof(text), "%u:%02u", totalSeconds / 60, totalSeconds % 60);
    DrawCenteredText(text, color, top);
}
