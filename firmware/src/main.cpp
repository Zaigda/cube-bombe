#include <Arduino.h>
#include "Config.h"
#include "Core/BombCode.h"
#include "Core/Palette.h"
#include "Enigma/Enigma.h"
#include "Enigma/SnakeEnigma.h"
#include "Inputs/InputManager.h"
#include "Inputs/PinButton.h"
#include "Outputs/PanelOutput.h"
#include "Outputs/Screen.h"

PinButton leftButton(Pins::SNAKE_LEFT_BUTTON);
PinButton rightButton(Pins::SNAKE_RIGHT_BUTTON);
InputManager inputs;

Screen snakeScreen(Panels::PANEL_WIDTH, Panels::SNAKE_PANELS * Panels::PANEL_HEIGHT, 0);
PanelOutput output(Pins::HUB75, Panels::PANEL_COUNT);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);

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
    Serial.println("Bomb cube - snake");

    inputs.Add(leftButton);
    inputs.Add(rightButton);
    inputs.Begin();

    code.Shuffle();
    snake.Start(millis());
}

void loop() {
    const uint32_t now = millis();

    inputs.Update(now);

    const EnigmaEvent event = snake.Update(now);
    if (event == EnigmaEvent::Mistake) {
        Serial.println("Mistake!");
    } else if (event == EnigmaEvent::Solved) {
        Serial.println("Solved! New code, new game.");
        code.Shuffle();
        snake.Start(now);
    }

    snakeScreen.fillScreen(Palette::BLACK);
    snake.Draw(now);
    output.Show();
}
