#include <Arduino.h>
#include "Config.h"
#include "Core/BombCode.h"
#include "Core/Palette.h"
#include "Enigma/CableEnigma.h"
#include "Enigma/Enigma.h"
#include "Enigma/SnakeEnigma.h"
#include "Inputs/CablePanel.h"
#include "Inputs/InputManager.h"
#include "Inputs/PinButton.h"
#include "Outputs/PanelOutput.h"
#include "Outputs/Screen.h"
#include "Outputs/StatusLed.h"

PinButton leftButton(Pins::SNAKE_LEFT_BUTTON);
PinButton rightButton(Pins::SNAKE_RIGHT_BUTTON);
PinButton resetButton(Pins::SNAKE_RESET_BUTTON);
CablePanel cables(Pins::CABLES, Pins::PLUGS);
InputManager inputs;

StatusLed statusLed(Pins::STATUS_LED);

Screen snakeScreen(Panels::PANEL_WIDTH, Panels::SNAKE_PANELS * Panels::PANEL_HEIGHT, 0);
PanelOutput output(Pins::HUB75, Panels::PANEL_COUNT);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);
CableEnigma cableEnigma(cables, snakeScreen, code);

enum class Phase : uint8_t { Paused, Snake, Failed, Cables, Solved };

Phase phase = Phase::Paused;
uint32_t failedMs = 0;

void NewGame(uint32_t now) {
    code.Shuffle();
    snake.Start(now);
    statusLed.Set(LedState::Off);
    phase = Phase::Paused;
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    output.AddScreen(snakeScreen);
    if (!output.Begin()) {
        Serial.println("Output failed to start: stopped");
        while (true) {
            delay(1000);
        }
    }
    Serial.println("Bomb cube - snake, press Start");

    inputs.Add(leftButton);
    inputs.Add(rightButton);
    inputs.Add(resetButton);
    inputs.Add(cables);
    inputs.Begin();
    statusLed.Begin();

    NewGame(millis());
}

void loop() {
    const uint32_t now = millis();

    inputs.Update(now);

    if (resetButton.WasPressed()) {
        if (phase == Phase::Paused) {
            Serial.println("Start");
            statusLed.Set(LedState::Blinking);
            phase = Phase::Snake;
        } else {
            Serial.println("Reset: new code, paused");
            NewGame(now);
        }
    }

    if (phase == Phase::Snake) {
        const EnigmaEvent event = snake.Update(now);
        if (event == EnigmaEvent::Mistake) {
            Serial.println("Mistake! Back to level 1");
            failedMs = now;
            phase = Phase::Failed;
        } else if (event == EnigmaEvent::Solved) {
            Serial.println("Snake done: plug the cables");
            cableEnigma.Start(now);
            phase = Phase::Cables;
        }
    } else if (phase == Phase::Failed && now - failedMs >= SnakeTuning::FAIL_MS) {
        phase = Phase::Snake;
    } else if (phase == Phase::Cables && cableEnigma.Update(now) == EnigmaEvent::Solved) {
        Serial.println("Cables right: enigma solved!");
        statusLed.Set(LedState::On);
        phase = Phase::Solved;
    }

    snakeScreen.fillScreen(Palette::BLACK);
    if (phase == Phase::Paused) {
        snake.Draw(now);
        snakeScreen.DrawCenteredText("START", Palette::WHITE, SnakeTuning::START_TEXT_TOP);
    } else if (phase == Phase::Snake) {
        snake.Draw(now);
    } else if (phase == Phase::Failed) {
        snakeScreen.fillScreen(Palette::RED);
        snakeScreen.DrawCenteredText("RATE", Palette::WHITE);
    } else if (phase == Phase::Cables) {
        cableEnigma.Draw(now);
    } else {
        snakeScreen.DrawCenteredText("DESAMORCE", Palette::GREEN);
    }
    output.Show();
    statusLed.Update(now);
}
