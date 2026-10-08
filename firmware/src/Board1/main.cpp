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
#include "Board1/Inputs/EitherButton.h"
#include "Board1/Inputs/InputManager.h"
#include "Board1/Inputs/PinButton.h"
#include "Board1/Inputs/RemoteButton.h"
#include "Board1/Link/BoardLink.h"
#include "Board1/Outputs/PanelOutput.h"
#include "Board1/Outputs/RemoteLed.h"
#include "Board1/Outputs/Screen.h"
#include "Board1/Outputs/StatusLed.h"
#include "Common/InputMessage.h"

PinButton leftButton(Pins::SNAKE_LEFT_BUTTON);
PinButton rightButton(Pins::SNAKE_RIGHT_BUTTON);
PinButton snakeStartButton(Pins::SNAKE_RESET_BUTTON);
CablePanel cables(Pins::CABLES, Pins::PLUGS);

RemoteButton invadersStartButton;
RemoteButton leftMic;
RemoteButton rightMic;
EitherButton shipLeft(leftMic, leftButton);
EitherButton shipRight(rightMic, rightButton);

StatusLed snakeLed(Pins::STATUS_LED);
RemoteLed invadersLed;

BoardLink boardLink(Serial2, invadersStartButton, leftMic, rightMic, invadersLed);
InputManager inputs;

Screen snakeScreen(Panels::PANEL_WIDTH, Panels::SNAKE_PANELS * Panels::PANEL_HEIGHT, 0);
Screen guideScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::GUIDE_PANEL);
Screen singerScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::SINGER_PANEL);
PanelOutput output(Pins::HUB75, Panels::PANEL_COUNT);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);
CableEnigma cableEnigma(cables, snakeScreen, code);
SnakeCablesEnigma snakeCables(snake, cableEnigma, code);
InvadersEnigma invaders(shipLeft, shipRight, guideScreen, singerScreen);

EnigmaStation snakeStation("Snake", snakeCables, snakeStartButton, snakeLed, snakeScreen);
EnigmaStation invadersStation("Invaders", invaders, invadersStartButton, invadersLed, guideScreen);

void setup() {
    Serial.begin(115200);
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
    inputs.Add(cables);
    inputs.Add(boardLink);
    inputs.Add(invadersStartButton);
    inputs.Add(leftMic);
    inputs.Add(rightMic);
    inputs.Begin();

    snakeStation.Begin(millis());
    invadersStation.Begin(millis());
}

void loop() {
    const uint32_t now = millis();

    inputs.Update(now);
    snakeStation.Update(now);
    invadersStation.Update(now);
    boardLink.SetSoundHz(invadersStation.IsPlaying() ? invaders.GetSoundHz(now) : 0);

    snakeScreen.fillScreen(Palette::BLACK);
    guideScreen.fillScreen(Palette::BLACK);
    singerScreen.fillScreen(Palette::BLACK);
    snakeStation.Draw(now);
    invadersStation.Draw(now);
    output.Show();

    // Get inputs from ESP32 - Board B
    if (Serial2.available() >= sizeof(InputMessage))
    {
        InputMessage msg;

        size_t received = Serial2.readBytes(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));

        if (received == sizeof(msg)) {
            Serial.printf("Reset Button: %u\n", msg.resetBtn);
            Serial.printf("Left Button: %u\n", msg.leftBtn);
            Serial.printf("Right Button: %u\n", msg.rightBtn);
        }
    }
}
