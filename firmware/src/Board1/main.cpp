#include <Arduino.h>
#include <iterator>
#include "Config.h"
#include "Board1/Core/BombCode.h"
#include "Board1/Core/BombDisplay.h"
#include "Board1/Core/Countdown.h"
#include "Board1/Core/GameManager.h"
#include "Board1/Core/Palette.h"
#include "Board1/Enigma/CableEnigma.h"
#include "Board1/Enigma/EnigmaStation.h"
#include "Board1/Enigma/InvadersEnigma.h"
#include "Board1/Enigma/SnakeCablesEnigma.h"
#include "Board1/Enigma/SnakeEnigma.h"
#include "Board1/Enigma/LightEnigma.h"
#include "Board1/Outputs/PanelOutput.h"
#include "Board1/Outputs/Screen.h"
#include "Common/Inputs/CablePanel.h"
#include "Common/Inputs/InputManager.h"
#include "Common/Inputs/PinButton.h"
#include "Common/Inputs/RemoteButton.h"
#include "Common/Outputs/StatusLed.h"
#include "Common/Messages/Enigma2Message.h"
#include "Common/Messages/Enigma3Message.h"
#include "Common/Messages/MessageLink.h"
#include "Common/Config.h"

constexpr uint16_t ALL_GRID_LEDS = 0b111111111;
constexpr uint32_t PANEL_ERROR_PRINT_MS = 1000;

PinButton leftButton(Pins::SNAKE_LEFT_BUTTON);
PinButton rightButton(Pins::SNAKE_RIGHT_BUTTON);
PinButton snakeStartButton(Pins::SNAKE_RESET_BUTTON, GameTuning::RESET_FEEDBACK_AFTER_MS);
RemoteButton invadersStartButton;
RemoteButton shipLeftButton;
RemoteButton shipRightButton;
RemoteButton lightStartButton;
CablePanel cables(Pins::CABLES, Pins::PLUGS);
InputManager inputs;

StatusLed snakeLed(Pins::STATUS_LED);

Screen snakeScreen(Panels::PANEL_WIDTH, Panels::SNAKE_PANELS * Panels::PANEL_HEIGHT, 0);
Screen guideScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::GUIDE_PANEL);
Screen singerScreen(Panels::PANEL_WIDTH, Panels::PANEL_HEIGHT, Panels::SINGER_PANEL);
PanelOutput output(Pins::HUB75, Panels::PANEL_COUNT);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);
CableEnigma cableEnigma(cables, snakeScreen, code);
SnakeCablesEnigma snakeCables(snake, cableEnigma, code);
InvadersEnigma invaders(shipLeftButton, shipRightButton, guideScreen, singerScreen);

MessageReader<Enigma3InputMessage> boardCReader;
Enigma3InputMessage boardC = {};
LightEnigma light(boardC);

Countdown countdown;
EnigmaStation snakeStation("Snake", snakeCables, snakeStartButton, countdown);
EnigmaStation invadersStation("Invaders", invaders, invadersStartButton, countdown);
EnigmaStation lightStation("Light", light, lightStartButton, countdown);
GameManager game(countdown, snakeStartButton);
BombDisplay bombDisplay(game, snakeScreen);

MessageReader<Enigma2InputMessage> boardBReader;
Enigma2InputMessage boardB = {};
uint32_t lastSendMs = 0;

void ReceiveFromBoardB(uint32_t now) {
    boardBReader.Receive(Serial2, boardB, now);
    invadersStartButton.SetDown(boardB.resetBtn);
    shipLeftButton.SetDown(boardB.leftBtn);
    shipRightButton.SetDown(boardB.rightBtn);
}

void ReceiveFromBoardC(uint32_t now) {
    boardCReader.Receive(Serial1, boardC, now);
    lightStartButton.SetDown(boardC.resetBtn);
}

uint16_t GetGridMask(uint32_t now) {
    if (game.GetState() == GameState::Defused) {
        const uint32_t step = now / EndTuning::CHASE_STEP_MS;
        return 1 << EndTuning::CHASE_ORDER[step % std::size(EndTuning::CHASE_ORDER)];
    }
    if (game.GetState() == GameState::Exploded) {
        return StatusLed::IsLit(LedState::FastBlinking, now) ? ALL_GRID_LEDS : 0;
    }
    if (lightStation.IsInFinalAlarm(now)) {
        return EnigmaStation::IsFinalAlarmLit(now) ? ALL_GRID_LEDS : 0;
    }
    if (lightStation.IsSolved()) {
        return ALL_GRID_LEDS;
    }
    if (!lightStation.IsStarted()) {
        return now % GameTuning::PULSE_MS < GameTuning::PULSE_MS / 2 ? 1 << LightTuning::IDLE_LED : 0;
    }
    return lightStation.IsPlaying() ? light.GetLedMask() : 0;
}

void SendToBoards(uint32_t now) {
    if (now - lastSendMs < LinkTuning::SEND_MS) {
        return;
    }
    lastSendMs = now;

    const Enigma2OutputMessage message = {
        .ledState = invadersStation.GetLedState()
    };
    MessageLink::Send(Serial2, message);

    const Enigma3OutputMessage lightMessage = {GetGridMask(now), lightStation.GetLedState()};
    MessageLink::Send(Serial1, lightMessage);
}

void DrawLinkCheck(uint32_t now) {
    if (!boardBReader.IsLinked(now)) {
        snakeScreen.DrawCenteredText(Texts::BOARD_2, Palette::RED, StationTuning::LINK_CHECK_TOP);
    }
    if (!boardCReader.IsLinked(now)) {
        snakeScreen.DrawCenteredText(Texts::BOARD_3, Palette::RED,
                                     StationTuning::LINK_CHECK_TOP + StationTuning::LINK_CHECK_LINE);
    }
}

void DrawScreens(uint32_t now) {
    snakeScreen.fillScreen(Palette::BLACK);
    guideScreen.fillScreen(Palette::BLACK);
    singerScreen.fillScreen(Palette::BLACK);
    if (!game.IsOver()) {
        snakeStation.Draw(now);
        invadersStation.Draw(now);
        lightStation.Draw(now);
    }
    if (game.GetState() == GameState::Waiting) {
        DrawLinkCheck(now);
    }
    bombDisplay.Draw(now);
    output.Show();
}

void ReportPanelError() {
    snakeLed.Set(LedState::FastBlinking);
    uint32_t lastPrintMs = millis() - PANEL_ERROR_PRINT_MS;
    while (true) {
        const uint32_t now = millis();
        if (now - lastPrintMs >= PANEL_ERROR_PRINT_MS) {
            lastPrintMs = now;
            Serial.printf("Panels failed to start: %s\n", output.GetError());
        }
        snakeLed.Update(now);
        delay(10);
    }
}

void setup() {
    Serial.begin(115200);
    snakeLed.Begin();
    output.AddScreen(snakeScreen);
    output.AddScreen(guideScreen);
    output.AddScreen(singerScreen);
    const bool outputStarted = output.Begin();
    if (outputStarted) {
        output.ShowPanelNumbers();
    }

    Serial2.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LINK_RX, Pins::LINK_TX);
    Serial1.begin(LinkTuning::BAUD, SERIAL_8N1, Pins::LIGHT_LINK_RX, Pins::LIGHT_LINK_TX);
    delay(2000);

    if (!outputStarted) {
        ReportPanelError();
    }
    Serial.println("Bomb cube - board 1");

    inputs.Add(leftButton);
    inputs.Add(rightButton);
    inputs.Add(snakeStartButton);
    inputs.Add(invadersStartButton);
    inputs.Add(shipLeftButton);
    inputs.Add(shipRightButton);
    inputs.Add(lightStartButton);
    inputs.Add(cables);
    inputs.Begin();

    snakeStation.AddScreen(snakeScreen);
    invadersStation.AddScreen(guideScreen);
    invadersStation.AddScreen(singerScreen);
    game.AddStation(snakeStation);
    game.AddStation(invadersStation);
    game.AddStation(lightStation);
    game.Begin(millis());

    bombDisplay.AddScreen(snakeScreen);
    bombDisplay.AddScreen(guideScreen);
    bombDisplay.AddScreen(singerScreen);
}

void loop() {
    const uint32_t now = millis();

    ReceiveFromBoardB(now);
    ReceiveFromBoardC(now);

    inputs.Update(now);
    game.Update(now);
    snakeLed.Set(snakeStation.GetLedState());
    snakeLed.Update(now);

    DrawScreens(now);
    SendToBoards(now);
}
