#include <Arduino.h>
#include "Config.h"

String linkLine;

void setup() {
    Serial.begin(115200);
    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);
    delay(2000);
    Serial.println("Board 2 ready");
}

void loop() {
    while (Serial2.available()) {
        const char c = static_cast<char>(Serial2.read());
        if (c == '\n') {
            linkLine.trim();
            Serial.printf("A -> B : %s\n", linkLine.c_str());
            linkLine = "";

            Serial2.println("PONG");
            Serial.println("B -> A : PONG");
        } else {
            linkLine += c;
        }
    }
}
