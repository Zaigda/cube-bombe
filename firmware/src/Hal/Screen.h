//
// Created by Paul Hugy on 06/10/2026.
//

#ifndef FIRMWARE_SCREEN_H
#define FIRMWARE_SCREEN_H
#include <cstdint>

// The game's fixed palette. Each screen translates it its own way:
// a character in the serial monitor, an RGB value on the LED panel.
enum class Color : uint8_t {
    Black,
    White,
    Red,
    Green,
    Blue,
    Yellow
};

// A grid of cells the game draws on. The game never knows how big a cell
// really is: one character in the serial monitor, 4x4 pixels on the panel.
class Screen {
public:
    virtual ~Screen() = default;

    virtual uint8_t Width() const = 0;
    virtual uint8_t Height() const = 0;

    virtual void Clear() = 0;
    virtual void SetCell(uint8_t x, uint8_t y, Color color) = 0;
    virtual void Show() = 0;    // pushes the frame, only if it changed
};


#endif //FIRMWARE_SCREEN_H
