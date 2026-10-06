#pragma once
#include "Screen.h"

class Output {
public:
    virtual ~Output() = default;

    void AddScreen(Screen &screen);

    virtual bool Begin() = 0;

    virtual void Show() = 0;

protected:
    static constexpr uint8_t MAX_SCREENS = 6;
    Screen *screens[MAX_SCREENS] = {};
    uint8_t screenCount = 0;
};
