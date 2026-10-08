#include <Arduino.h>
#include "Common/InputMessage.h"
#include "Common/MessageLink.h"
#include "Common/OutputMessage.h"

constexpr uint8_t RX_PIN = 21;
constexpr uint8_t TX_PIN = 1;

constexpr uint8_t INPUT_RESET_BUTTON = 4;

constexpr uint32_t SEND_MS = 50;

uint32_t lastSendMs = 0;

MessageReader<OutputMessage> boardAReader;
OutputMessage boardA = {};

void setup()
{
    Serial.begin(115200);

    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

    Serial.println("ESP32 - Board B - Initialized.");

    pinMode(INPUT_RESET_BUTTON, INPUT_PULLUP);
}

void loop()
{
    const uint32_t now = millis();

    OutputMessage received;
    if (boardAReader.Read(Serial2, received))
    {
        if (received.ledState != boardA.ledState || received.soundHz != boardA.soundHz)
        {
            Serial.printf("A -> B : led %u, sound %u Hz\n", received.ledState, received.soundHz);
        }
        boardA = received;
    }

    if (now - lastSendMs < SEND_MS)
    {
        return;
    }
    lastSendMs = now;

    InputMessage inputMessage = {
        .resetBtn = digitalRead(INPUT_RESET_BUTTON) == LOW,
        .left = false,
        .right = false
    };

    MessageLink::Send(Serial2, inputMessage);
}
