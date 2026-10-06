#include "Output.h"
#include <Arduino.h>

void Output::AddScreen(Screen &screen) {
    if (screenCount >= MAX_SCREENS) {
        Serial.println("Output: too many screens, increase MAX_SCREENS");
        return;
    }
    screens[screenCount] = &screen;
    screenCount++;
}
