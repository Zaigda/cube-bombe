#include "PanelOutput.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

PanelOutput::PanelOutput(const Pins::Hub75 &hub75Pins, uint8_t chainLength)
    : pins(hub75Pins), panelCount(chainLength) {
}

bool PanelOutput::Begin() {
    if (panelCount == 0 || panelCount > Panels::MAX_PANELS) {
        Serial.printf("PanelOutput: %u panels requested, between 1 and %u allowed\n", panelCount,
                      Panels::MAX_PANELS);
        return false;
    }
    bool panelUsed[Panels::MAX_PANELS] = {};
    for (uint8_t i = 0; i < screenCount; i++) {
        if (!CheckScreen(*screens[i], i, panelUsed)) {
            return false;
        }
    }

    const HUB75_I2S_CFG::i2s_pins libraryPins = {
        pins.r1, pins.g1, pins.b1, pins.r2, pins.g2, pins.b2,
        pins.a, pins.b, pins.c, pins.d, pins.e,
        pins.lat, pins.oe, pins.clk
    };
    HUB75_I2S_CFG config(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, panelCount, libraryPins);
    config.double_buff = true;

    panels = new MatrixPanel_I2S_DMA(config);
    if (!panels->begin()) {
        Serial.println("PanelOutput: begin() failed (not enough DMA memory?)");
        delete panels;
        panels = nullptr;
        return false;
    }
    panels->setBrightness8(Panels::BRIGHTNESS);
    return true;
}

void PanelOutput::Show() {
    if (panels == nullptr) {
        return;
    }
    const uint32_t now = millis();
    if (now - lastFrameMs < Panels::FRAME_MS) {
        return;
    }
    lastFrameMs = now;

    for (uint8_t i = 0; i < screenCount; i++) {
        const Screen &screen = *screens[i];
        const uint8_t sliceCount = screen.height() / Panels::PANEL_HEIGHT;
        for (uint8_t slice = 0; slice < sliceCount; slice++) {
            DrawSlice(screen, slice);
        }
    }
    panels->flipDMABuffer();
}

bool PanelOutput::CheckScreen(const Screen &screen, uint8_t index, bool panelUsed[]) const {
    if (!screen.HasPixels()) {
        Serial.printf("PanelOutput: screen %u has no pixels (not enough memory)\n", index);
        return false;
    }
    if (screen.width() != Panels::PANEL_WIDTH || screen.height() % Panels::PANEL_HEIGHT != 0) {
        Serial.printf("PanelOutput: screen %u must be %u wide and a multiple of %u high\n", index, Panels::PANEL_WIDTH,
                      Panels::PANEL_HEIGHT);
        return false;
    }
    const int panelsNeeded = screen.GetFirstPanel() + screen.height() / Panels::PANEL_HEIGHT;
    if (panelsNeeded > panelCount) {
        Serial.printf("PanelOutput: screen %u needs %d panels, the chain has %u\n", index, panelsNeeded, panelCount);
        return false;
    }
    for (int panel = screen.GetFirstPanel(); panel < panelsNeeded; panel++) {
        if (panelUsed[panel]) {
            Serial.printf("PanelOutput: screen %u is on panel %d, already used by another screen\n", index, panel);
            return false;
        }
        panelUsed[panel] = true;
    }
    return true;
}

void PanelOutput::DrawSlice(const Screen &screen, uint8_t slice) {
    const uint8_t panel = screen.GetFirstPanel() + slice;
    const int16_t panelX = (panelCount - 1 - panel) * Panels::PANEL_WIDTH;
    const uint16_t *pixels = screen.getBuffer() + slice * Panels::PANEL_WIDTH * Panels::PANEL_HEIGHT;

    if (!Panels::ROTATED_180[panel]) {
        panels->drawRGBBitmap(panelX, 0, pixels, Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT);
        return;
    }

    for (int16_t y = 0; y < Panels::PANEL_HEIGHT; y++) {
        for (int16_t x = 0; x < Panels::PANEL_WIDTH; x++) {
            const uint16_t color = pixels[y * Panels::PANEL_WIDTH + x];
            panels->drawPixel(panelX + Panels::PANEL_WIDTH - 1 - x, Panels::PANEL_HEIGHT - 1 - y, color);
        }
    }
}
