#include "SerialOutput.h"
#include <Arduino.h>
#include "Core/Palette.h"

static const char *CLEAR_TERMINAL = "\033[2J";
static const char *CURSOR_HOME = "\033[H";

static uint32_t Fingerprint(const Screen &screen) {
    const uint16_t *pixels = screen.getBuffer();
    const uint32_t pixelCount = screen.width() * screen.height();

    uint32_t fingerprint = 0;
    for (uint32_t i = 0; i < pixelCount; i++) {
        fingerprint = fingerprint * 31 + pixels[i];
    }
    return fingerprint;
}

static char Glyph(uint16_t color) {
    switch (color) {
        case Palette::BLACK: return ' ';
        case Palette::WHITE: return '#';
        case Palette::RED: return 'R';
        case Palette::GREEN: return 'G';
        case Palette::BLUE: return 'B';
        case Palette::YELLOW: return 'Y';
        case Palette::ORANGE: return 'O';
        default: return '+';
    }
}

static void PrintBorder(int16_t width) {
    Serial.print('+');
    for (int16_t x = 0; x < width; x++) {
        Serial.print('-');
    }
    Serial.println('+');
}

bool SerialOutput::Begin() {
    for (uint8_t i = 0; i < screenCount; i++) {
        if (!screens[i]->HasPixels()) {
            Serial.printf("SerialOutput: screen %u has no pixels (not enough memory)\n", i);
            return false;
        }
    }
    Serial.print(CLEAR_TERMINAL);
    return true;
}

void SerialOutput::Show() {
    bool changed = false;
    for (uint8_t i = 0; i < screenCount; i++) {
        const uint32_t fingerprint = Fingerprint(*screens[i]);
        if (fingerprint != lastFingerprints[i]) {
            lastFingerprints[i] = fingerprint;
            changed = true;
        }
    }
    if (!changed) {
        return;
    }

    Serial.print(CURSOR_HOME);
    for (uint8_t i = 0; i < screenCount; i++) {
        Print(*screens[i], i);
    }
}

void SerialOutput::Print(const Screen &screen, uint8_t index) const {
    const int16_t width = screen.width();
    const int16_t height = screen.height();

    Serial.printf("Screen %u\n", index);
    PrintBorder(width);
    for (int16_t y = 0; y < height; y += 2) {
        Serial.print('|');
        for (int16_t x = 0; x < width; x++) {
            const uint16_t top = screen.getPixel(x, y);
            const uint16_t bottom = screen.getPixel(x, y + 1);
            Serial.print(Glyph(top != Palette::BLACK ? top : bottom));
        }
        Serial.println('|');
    }
    PrintBorder(width);
}
