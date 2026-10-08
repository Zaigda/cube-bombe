#include <Arduino.h>
#include "Config.h"
#include "Common/Config.h"
#include "Common/Messages/Enigma3Message.h"
#include "Common/Messages/MessageLink.h"

MessageReader<Enigma3OutputMessage> boardAReader;
Enigma3OutputMessage boardA = {};
uint32_t lastBoardAMs = 0;
uint32_t lastSendMs = 0;

uint16_t ReadLight(uint8_t pin) {
    uint32_t sum = 0;
    for (int i = 0; i < LightTuning::SAMPLES; i++) {
        sum += analogRead(pin);
    }
    return sum / LightTuning::SAMPLES;
}

void SendSensors() {
    Enigma3InputMessage message = {};
    message.resetBtn = digitalRead(Pins::START_BUTTON) == LOW;
    for (uint8_t i = 0; i < LIGHT_SENSOR_COUNT; i++) {
        message.light[i] = ReadLight(Pins::LIGHT_SENSORS[i]);
    }
    MessageLink::Send(Serial2, message);
}

void ShowLeds(uint32_t now) {
    if (now - lastBoardAMs >= LinkTuning::TIMEOUT_MS) {
        boardA = {};
    }
    for (uint8_t i = 0; i < LIGHT_SENSOR_COUNT; i++) {
        digitalWrite(Pins::GRID_LEDS[i], (boardA.ledMask >> i) & 1 ? HIGH : LOW);
    }
    const bool blinkOn = (now / StatusLight::BLINK_MS) % 2 == 0;
    const bool statusOn = boardA.ledState == 2 || (boardA.ledState == 1 && blinkOn);
    digitalWrite(Pins::STATUS_LED, statusOn ? HIGH : LOW);
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    pinMode(Pins::START_BUTTON, INPUT_PULLUP);
    pinMode(Pins::STATUS_LED, OUTPUT);
    for (uint8_t pin : Pins::GRID_LEDS) {
        pinMode(pin, OUTPUT);
    }
    Serial.println("Bomb cube - board 3 (light sensors)");
}

void loop() {
    const uint32_t now = millis();

    if (boardAReader.Read(Serial2, boardA)) {
        lastBoardAMs = now;
    }
    ShowLeds(now);

    if (now - lastSendMs >= LinkTuning::SEND_MS) {
        lastSendMs = now;
        SendSensors();
    }
}
