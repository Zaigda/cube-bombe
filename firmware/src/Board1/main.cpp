#include <Arduino.h>
#include "Config.h"
#include "Board1/Core/BombCode.h"
#include "Board1/Core/Palette.h"
#include "Board1/Enigma/CableEnigma.h"
#include "Board1/Enigma/EnigmaStation.h"
#include "Board1/Enigma/InvadersEnigma.h"
#include "Board1/Enigma/SnakeCablesEnigma.h"
#include "Board1/Enigma/SnakeEnigma.h"
#include "Board1/Inputs/CablePanel.h"
#include "Board1/Inputs/InputManager.h"
#include "Board1/Inputs/PinButton.h"
#include "Board1/Inputs/RemoteButton.h"
#include "Board1/Outputs/PanelOutput.h"
#include "Board1/Outputs/RemoteLed.h"
#include "Board1/Outputs/Screen.h"
#include "Board1/Outputs/StatusLed.h"
#include "Common/InputMessage.h"
#include "Common/MessageLink.h"
#include "Common/OutputMessage.h"

PinButton leftButton(Pins::SNAKE_LEFT_BUTTON);
PinButton rightButton(Pins::SNAKE_RIGHT_BUTTON);
PinButton snakeStartButton(Pins::SNAKE_RESET_BUTTON);
RemoteButton invadersStartButton;
RemoteButton shipLeftButton;
RemoteButton shipRightButton;
CablePanel cables(Pins::CABLES, Pins::PLUGS);
InputManager inputs;

StatusLed snakeLed(Pins::STATUS_LED);
RemoteLed invadersLed;

Screen snakeScreen(Panels::PANEL_WIDTH, Panels::SNAKE_PANELS * Panels::PANEL_HEIGHT, 0);
Screen guideScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::GUIDE_PANEL);
Screen singerScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::SINGER_PANEL);
PanelOutput output(Pins::HUB75, Panels::PANEL_COUNT);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);
CableEnigma cableEnigma(cables, snakeScreen, code);
SnakeCablesEnigma snakeCables(snake, cableEnigma, code);
InvadersEnigma invaders(shipLeftButton, shipRightButton, guideScreen, singerScreen);

EnigmaStation snakeStation("Snake", snakeCables, snakeStartButton, snakeLed, snakeScreen);
EnigmaStation invadersStation("Invaders", invaders, invadersStartButton, invadersLed, guideScreen);

MessageReader<InputMessage> boardBReader;
InputMessage boardB = {};
uint32_t lastBoardBMs = 0;
uint32_t lastSendMs = 0;

void ReadBoardB(uint32_t now) {
    if (boardBReader.Read(Serial2, boardB)) {
        lastBoardBMs = now;
    }
    if (now - lastBoardBMs >= LinkTuning::TIMEOUT_MS) {
        boardB = {};
    }
    invadersStartButton.SetDown(boardB.resetBtn);
    shipLeftButton.SetDown(boardB.leftBtn || leftButton.IsDown());
    shipRightButton.SetDown(boardB.rightBtn || rightButton.IsDown());
}

void SendToBoardB(uint32_t now) {
    if (now - lastSendMs < LinkTuning::SEND_MS) {
        return;
    }
    lastSendMs = now;

    const uint16_t soundHz = invadersStation.IsPlaying() ? invaders.GetSoundHz(now) : 0;
    const OutputMessage message = {static_cast<uint8_t>(invadersLed.GetState()), soundHz};
    MessageLink::Send(Serial2, message);
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);
    delay(2000);

    output.AddScreen(snakeScreen);
    output.AddScreen(guideScreen);
    output.AddScreen(singerScreen);
    if (!output.Begin()) {
        Serial.println("Output failed to start: stopped");
        while (true) {
            delay(1000);
        }
    }
    Serial.println("Bomb cube - board 1");

    inputs.Add(leftButton);
    inputs.Add(rightButton);
    inputs.Add(snakeStartButton);
    inputs.Add(invadersStartButton);
    inputs.Add(shipLeftButton);
    inputs.Add(shipRightButton);
    inputs.Add(cables);
    inputs.Begin();

    snakeStation.Begin(millis());
    invadersStation.Begin(millis());
}

void loop() {
    const uint32_t now = millis();

    ReadBoardB(now);

    inputs.Update(now);
    snakeStation.Update(now);
    invadersStation.Update(now);

    snakeScreen.fillScreen(Palette::BLACK);
    guideScreen.fillScreen(Palette::BLACK);
    singerScreen.fillScreen(Palette::BLACK);
    snakeStation.Draw(now);
    invadersStation.Draw(now);
    output.Show();

    SendToBoardB(now);
}
