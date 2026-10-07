#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "Config.h"

MatrixPanel_I2S_DMA *panel = nullptr;
bool panelStarted = false;
uint32_t lastPrintMs = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);

    const HUB75_I2S_CFG::i2s_pins pins = {
        Pins::HUB75.r1, Pins::HUB75.g1, Pins::HUB75.b1, Pins::HUB75.r2, Pins::HUB75.g2, Pins::HUB75.b2,
        Pins::HUB75.a, Pins::HUB75.b, Pins::HUB75.c, Pins::HUB75.d, Pins::HUB75.e,
        Pins::HUB75.lat, Pins::HUB75.oe, Pins::HUB75.clk
    };
    HUB75_I2S_CFG config(64, 32, 2, pins);
    panel = new MatrixPanel_I2S_DMA(config);
    panelStarted = panel->begin();
    if (panelStarted) {
        panel->setBrightness8(60);
        panel->fillScreen(panel->color565(255, 0, 0));
    }
}

void loop() {
    const uint32_t now = millis();
    if (now - lastPrintMs >= 1000) {
        lastPrintMs = now;
        Serial.println(panelStarted ? "Panel test: all red" : "Panel begin failed");
    }
}
