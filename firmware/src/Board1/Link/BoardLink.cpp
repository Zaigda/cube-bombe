#include "BoardLink.h"
#include "Board1/Config.h"

BoardLink::BoardLink(HardwareSerial &linkSerial, RemoteButton &startButton, RemoteButton &leftButton,
                     RemoteButton &rightButton, const RemoteLed &statusLed)
    : serial(linkSerial), start(startButton), left(leftButton), right(rightButton), led(statusLed) {
}

void BoardLink::Begin() {
    serial.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);
}

void BoardLink::Update(uint32_t now) {
    while (serial.available()) {
        const char c = static_cast<char>(serial.read());
        if (c == '\n') {
            line.trim();
            Apply(line, now);
            line = "";
        } else if (line.length() < LinkTuning::MAX_LINE) {
            line += c;
        }
    }

    if (now - lastReceiveMs >= LinkTuning::TIMEOUT_MS) {
        start.SetDown(false);
        left.SetDown(false);
        right.SetDown(false);
    }

    if (now - lastSendMs >= LinkTuning::SEND_MS) {
        lastSendMs = now;
        serial.printf("A %d %u\n", static_cast<int>(led.GetState()), soundHz);
    }
}

void BoardLink::SetSoundHz(uint16_t hz) {
    soundHz = hz;
}

void BoardLink::Apply(const String &message, uint32_t now) {
    int startDown = 0;
    char direction = 'N';
    if (sscanf(message.c_str(), "B %d %c", &startDown, &direction) != 2) {
        Serial.printf("Link: ignored \"%s\"\n", message.c_str());
        return;
    }
    lastReceiveMs = now;
    start.SetDown(startDown == 1);
    left.SetDown(direction == 'L');
    right.SetDown(direction == 'R');
}
