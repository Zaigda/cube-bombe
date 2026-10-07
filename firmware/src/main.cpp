#include <Arduino.h>
#include "Config.h"
#include "Core/BombCode.h"
#include "Core/Palette.h"
#include "Enigma/Enigma.h"
#include "Enigma/SnakeEnigma.h"
#include "Inputs/InputManager.h"
#include "Inputs/KeyButton.h"
#include "Inputs/SerialKeys.h"
#include "Outputs/PanelOutput.h"
#include "Outputs/Screen.h"
#include "Outputs/SerialOutput.h"

SerialKeys keys;
KeyButton leftButton(keys, 'q');
KeyButton rightButton(keys, 'd');
InputManager inputs;

#if defined(CUBE_HARDWARE)
Screen snakeScreen(64, 64, 0);
PanelOutput output(Pins::HUB75_CUBE, 2);
#elif defined(MINI_PANEL)
Screen snakeScreen(64, 64, 0);
PanelOutput output(Pins::HUB75_MINI, 2);
#else
Screen snakeScreen(64, 64, 0);
SerialOutput output;
#endif

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
    Serial.println("Bomb cube - step 4: snake");

    inputs.Add(keys);
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
