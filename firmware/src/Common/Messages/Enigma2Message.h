#pragma once

#include <Arduino.h>
#include "Common/Outputs/Led.h"

struct Enigma2InputMessage
{
    bool resetBtn;
    bool leftBtn;
    bool rightBtn;
};

struct Enigma2OutputMessage
{
    LedState ledState;
};