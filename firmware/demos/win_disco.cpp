#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA *panel = nullptr;
uint32_t lastFrameMs = 0;

static uint16_t Hue(uint8_t hue, uint8_t value) {
    const uint8_t region = hue / 43;
    const uint8_t rest = (hue - region * 43) * 6;
    const uint8_t falling = (value * (255 - rest)) >> 8;
    const uint8_t rising = (value * rest) >> 8;
    switch (region) {
        case 0: return panel->color565(value, rising, 0);
        case 1: return panel->color565(falling, value, 0);
        case 2: return panel->color565(0, value, rising);
        case 3: return panel->color565(0, falling, value);
        case 4: return panel->color565(rising, 0, value);
        default: return panel->color565(value, 0, falling);
    }
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    const HUB75_I2S_CFG::i2s_pins pins = {1, 2, 4, 5, 6, 7, 8, 9, 3, 11, -1, 13, 44, 12};
    HUB75_I2S_CFG config(64, 32, 1, pins);
    config.double_buff = true;
    panel = new MatrixPanel_I2S_DMA(config);
    if (!panel->begin()) {
        Serial.println("Panel begin failed");
        while (true) {
            delay(1000);
        }
    }
    panel->setBrightness8(80);
    panel->setTextSize(2);
}

void loop() {
    const uint32_t now = millis();
    if (now - lastFrameMs < 20) {
        return;
    }
    lastFrameMs = now;

    const uint8_t time = now / 8;

    for (int16_t y = 0; y < panel->height(); y++) {
        for (int16_t x = 0; x < panel->width(); x++) {
            const uint8_t hue = time + x * 4 + y * 6 + static_cast<uint8_t>(sin((x + time) * 0.15f) * 20);
            panel->drawPixel(x, y, Hue(hue, 70));
        }
    }

    const char *letters = "WIN";
    const int16_t letterWidth = 12;
    const int16_t startX = (panel->width() - 3 * letterWidth) / 2;
    const int16_t bounce = static_cast<int16_t>(sin(now * 0.008f) * 3);
    for (int i = 0; i < 3; i++) {
        const int16_t x = startX + i * letterWidth;
        const int16_t y = 9 + ((i % 2 == 0) ? bounce : -bounce);
        panel->setTextColor(0);
        panel->setCursor(x + 1, y + 1);
        panel->print(letters[i]);
        panel->setTextColor(Hue(time * 3 + i * 85, 255));
        panel->setCursor(x, y);
        panel->print(letters[i]);
    }

    panel->flipDMABuffer();
}
