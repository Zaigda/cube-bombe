#include <Arduino.h>
#include "Config.h"
#include "Microphone.h"
#include "Common/InputMessage.h"
#include "Common/MessageLink.h"
#include "Common/OutputMessage.h"

constexpr uint32_t SEND_MS = 50;

uint32_t lastSendMs = 0;
Microphone leftMicrophone(Pins::INPUT_LEFT_MICRO_ANALOG);
Microphone rightMicrophone(Pins::INPUT_RIGHT_MICRO_ANALOG);

MessageReader<OutputMessage> boardAReader;
OutputMessage boardA = {};

void setup()
{
    Serial.begin(115200);

    Serial2.begin(115200, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);

    Serial.println("ESP32 - Board B - Initialized.");

    pinMode(Pins::INPUT_RESET_BUTTON, INPUT_PULLUP);
    leftMicrophone.Begin();
    rightMicrophone.Begin();
}

void loop()
{
    leftMicrophone.Update();
    rightMicrophone.Update();

    const uint32_t now = millis();
    if (now - lastSendMs < SEND_MS)
    {
        return;
    }
    lastSendMs = now;

    InputMessage inputMessage = {
        .resetBtn = digitalRead(Pins::INPUT_RESET_BUTTON) == LOW,
        .leftBtn = leftMicrophone.Detected(),
        .rightBtn = rightMicrophone.Detected()
    };

    MessageLink::Send(Serial2, inputMessage);
}
