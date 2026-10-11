#include <Arduino.h>
#include "Microphone.h"
#include "Board2/Config.h"
#include "Common/Config.h"
#include "Common/Messages/Enigma2Message.h"
#include "Common/Messages/MessageLink.h"
#include "Common/Outputs/StatusLed.h"

StatusLed statusLed(Pins::STATUS_LED);

Microphone leftMicrophone(Pins::LEFT_MICROPHONE);
Microphone rightMicrophone(Pins::RIGHT_MICROPHONE);

MessageReader<Enigma2OutputMessage> board1Reader;
Enigma2OutputMessage board1 = {};
uint32_t lastSendMs = 0;

void ReceiveFromBoard1(uint32_t now) {
    board1Reader.Receive(Serial2, board1, now);
    statusLed.Set(board1.ledState);
}

// When both microphones hear a sound, only the louder one is sent (an exact tie goes to the left).
void SendToBoard1(uint32_t now) {
    if (now - lastSendMs < LinkTuning::SEND_MS) {
        return;
    }
    lastSendMs = now;

    Enigma2InputMessage message = {};
    message.startButton = digitalRead(Pins::START_BUTTON) == LOW;
    message.leftHeard = leftMicrophone.IsDetected();
    message.rightHeard = rightMicrophone.IsDetected();

    if (message.leftHeard && message.rightHeard) {
        message.leftHeard = leftMicrophone.GetLevel() >= rightMicrophone.GetLevel();
        message.rightHeard = !message.leftHeard;
    }

    MessageLink::Send(Serial2, message);
}

void setup() {
    Serial.begin(115200);

    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);

    Serial.println("Bomb cube - board 2 (microphones)");

    pinMode(Pins::START_BUTTON, INPUT_PULLUP);
    statusLed.Begin();
    leftMicrophone.Begin();
    rightMicrophone.Begin();
}

void loop() {
    const uint32_t now = millis();

    ReceiveFromBoard1(now);

    statusLed.Update(now);
    leftMicrophone.Update();
    rightMicrophone.Update();

    SendToBoard1(now);
}
