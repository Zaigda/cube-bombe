//
// Created by Paul Hugy on 06/10/2026.
//

#include "SerialScreen.h"
#include <Arduino.h>
#include <cstring>

// Two characters per cell: a terminal character is about twice as tall as it
// is wide, so doubling it makes the cells look square.
static const char* Glyph(Color color) {
    switch (color) {
        case Color::Black:  return "  ";
        case Color::White:  return "##";
        case Color::Red:    return "XX";
        case Color::Green:  return "@@";
        case Color::Blue:   return "[]";
        case Color::Yellow: return "**";
    }
    return "??";
}

static void PrintBorder(uint8_t width) {
    Serial.print('+');
    for (uint8_t x = 0; x < width; x++) {
        Serial.print("--");
    }
    Serial.println('+');
}

uint8_t SerialScreen::Width() const {
    return WIDTH;
}

uint8_t SerialScreen::Height() const {
    return HEIGHT;
}

void SerialScreen::Clear() {
    for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            cells[y][x] = Color::Black;
        }
    }
}

// Coordinates outside the grid are ignored: a bug in the game must never be
// able to write outside the array.
void SerialScreen::SetCell(uint8_t x, uint8_t y, Color color) {
    if (x >= WIDTH || y >= HEIGHT) {
        return;
    }
    cells[y][x] = color;
}

// loop() calls this thousands of times per second: only print when the frame
// differs from the one already in the monitor.
void SerialScreen::Show() {
    if (memcmp(cells, shown, sizeof(cells)) == 0) {
        return;
    }
    memcpy(shown, cells, sizeof(cells));

    PrintBorder(WIDTH);
    for (uint8_t y = 0; y < HEIGHT; y++) {
        Serial.print('|');
        for (uint8_t x = 0; x < WIDTH; x++) {
            Serial.print(Glyph(cells[y][x]));
        }
        Serial.println('|');
    }
    PrintBorder(WIDTH);
}
