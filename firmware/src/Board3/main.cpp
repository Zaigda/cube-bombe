#include <Arduino.h>
#include "Board3/Config.h"
#include "Common/Config.h"
#include "Common/Messages/Enigma3Message.h"
#include "Common/Messages/MessageLink.h"
#include "Common/Outputs/StatusLed.h"

StatusLed statusLed(Pins::STATUS_LED);

MessageReader<Enigma3OutputMessage> board1Reader;
Enigma3OutputMessage board1 = {};
uint32_t lastSendMs = 0;

uint16_t ReadLight(uint8_t pin) {
    uint32_t sum = 0;
    for (int i = 0; i < SensorTuning::SAMPLES; i++) {
        sum += analogRead(pin);
    }
    return sum / SensorTuning::SAMPLES;
}

void SendSensors() {
    Enigma3InputMessage message = {};
    message.startButton = digitalRead(Pins::START_BUTTON) == LOW;
    for (uint8_t i = 0; i < LIGHT_SENSOR_COUNT; i++) {
        message.light[i] = ReadLight(Pins::LIGHT_SENSORS[i]);
    }
    MessageLink::Send(Serial2, message);
}

void ShowLeds(uint32_t now) {
    for (uint8_t i = 0; i < LIGHT_SENSOR_COUNT; i++) {
        digitalWrite(Pins::GRID_LEDS[i], (board1.ledMask >> i) & 1 ? HIGH : LOW);
    }
    statusLed.Set(board1.ledState);
    statusLed.Update(now);
}

void SweepGridLeds() {
    for (uint8_t pin : Pins::GRID_LEDS) {
        digitalWrite(pin, HIGH);
        delay(GridTuning::SWEEP_STEP_MS);
        digitalWrite(pin, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    pinMode(Pins::START_BUTTON, INPUT_PULLUP);
    statusLed.Begin();
    for (uint8_t pin : Pins::GRID_LEDS) {
        pinMode(pin, OUTPUT);
    }
    SweepGridLeds();
    Serial.println("Bomb cube - board 3 (light sensors)");
}

void loop() {
    const uint32_t now = millis();

    board1Reader.Receive(Serial2, board1, now);
    ShowLeds(now);

    if (now - lastSendMs >= LinkTuning::SEND_MS) {
        lastSendMs = now;
        SendSensors();
    }
}
