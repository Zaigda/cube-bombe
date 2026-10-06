//
// Created by Paul Hugy on 06/10/2026.
//

#ifndef FIRMWARE_SERIALSCREEN_H
#define FIRMWARE_SERIALSCREEN_H
#include "Screen.h"


// Draws the grid as ASCII art in the serial monitor, so the whole game can be
// played before the LED panel is wired.
class SerialScreen : public Screen {
public:
    uint8_t Width() const override;
    uint8_t Height() const override;

    void Clear() override;
    void SetCell(uint8_t x, uint8_t y, Color color) override;
    void Show() override;

private:
    static constexpr uint8_t WIDTH = 16;
    static constexpr uint8_t HEIGHT = 8;

    Color cells[HEIGHT][WIDTH]{};   // the frame being drawn
    Color shown[HEIGHT][WIDTH]{};   // the frame currently in the monitor
};


#endif //FIRMWARE_SERIALSCREEN_H
