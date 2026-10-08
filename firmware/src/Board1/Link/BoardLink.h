#pragma once
#include <Arduino.h>
#include "Board1/Inputs/RemoteButton.h"
#include "Board1/Inputs/Sensor.h"
#include "Board1/Outputs/RemoteLed.h"

// The wire to board 2. Receives "B <start 0|1> <L|R|N>" (start button, which mic shouts) and sends
// "A <led 0|1|2> <note Hz, 0 = silence>" every SEND_MS. No message for TIMEOUT_MS releases the remote buttons.
class BoardLink : public Sensor {
public:
    BoardLink(HardwareSerial &linkSerial, RemoteButton &startButton, RemoteButton &leftButton, RemoteButton &rightButton,
              const RemoteLed &statusLed);

    void Begin() override;

    void Update(uint32_t now) override;

    void SetSoundHz(uint16_t hz);

private:
    void Apply(const String &message, uint32_t now);

    HardwareSerial &serial;
    RemoteButton &start;
    RemoteButton &left;
    RemoteButton &right;
    const RemoteLed &led;

    String line;
    uint16_t soundHz = 0;
    uint32_t lastSendMs = 0;
    uint32_t lastReceiveMs = 0;
};
