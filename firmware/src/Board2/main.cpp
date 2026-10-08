#include <Arduino.h>
#include "Common/InputMessage.h"

constexpr uint8_t RX_PIN = 21;
constexpr uint8_t TX_PIN = 1;

constexpr uint8_t INPUT_RESET_BUTTON = 4;

void setup()
{
    Serial.begin(115200);

    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

    Serial.println("ESP32 - Board B - Initialized.");

    pinMode(INPUT_RESET_BUTTON, INPUT_PULLUP);
}

void loop()
{
    InputMessage inputMessage = {
        .resetBtn = digitalRead(INPUT_RESET_BUTTON)
    };

    Serial2.write(reinterpret_cast<const uint8_t*>(&inputMessage), sizeof(inputMessage));

    delay(1000);
}
