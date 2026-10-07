#include "SerialKeyboard.h"

void SerialKeyboard::Update(uint32_t) {
    left.pressed = false;
    right.pressed = false;
    testRequested = false;

    // Limit this batch to bytes already waiting: continuous traffic cannot
    // prevent the game and display from updating.
    const int available = serial.available();
    for (int i = 0; i < available; i++) {
        switch (serial.read()) {
            case 'q':
            case 'Q':
                left.pressed = true;
                right.pressed = false;
                break;
            case 'd':
            case 'D':
                left.pressed = false;
                right.pressed = true;
                break;
            case 't':
            case 'T':
                testRequested = true;
                break;
            default:
                break;
        }
    }
}
