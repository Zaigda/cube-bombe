#include <Arduino.h>
#include "Config.h"
#include "Core/BombCode.h"
#include "Core/Diagnostics.h"
#include "Core/Palette.h"
#include "Enigma/Enigma.h"
#include "Enigma/SnakeEnigma.h"
#include "Inputs/InputManager.h"
#include "Inputs/KeyButton.h"
#include "Inputs/SerialKeys.h"
#include "Outputs/Screen.h"
#include "Outputs/SerialOutput.h"

SerialKeys keys;
KeyButton leftButton(keys, 'q');
KeyButton rightButton(keys, 'd');
InputManager inputs;

Screen snakeScreen(64, 64, 0);
SerialOutput output;

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);

Diagnostics diagnostics;

static void PrintMemoryMap() {
    const int localNumber = 42;

    Serial.println("--- Where things live in memory ---");
    Diagnostics::PrintMemory("snake (global)", &snake, sizeof(snake));
    Diagnostics::PrintMemory("snakeScreen (global)", &snakeScreen, sizeof(snakeScreen));
    Diagnostics::PrintMemory("snakeScreen pixels", snakeScreen.getBuffer(), 64 * 64 * 2);
    Diagnostics::PrintMemory("code (global)", &code, sizeof(code));
    Diagnostics::PrintMemory("localNumber (local)", &localNumber, sizeof(localNumber));
    Diagnostics::PrintMemory("\"text\" (literal)", "text", 5);
    Diagnostics::PrintFreeMemory();

    Serial.println("Press a key to start (or wait 20 s)");
    const uint32_t waitStartMs = millis();
    while (Serial.available() == 0 && millis() - waitStartMs < 20000) {
        delay(10);
    }
    while (Serial.available() > 0) {
        Serial.read();
    }
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
    Serial.println("Bomb cube - step 4: snake");

    inputs.Add(keys);
    inputs.Add(leftButton);
    inputs.Add(rightButton);
    inputs.Begin();

    if (Debug::DIAGNOSTICS) {
        PrintMemoryMap();
    }

    code.Shuffle();
    snake.Start(millis());
}

void loop() {
    diagnostics.BeginLoop();
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

    if (Debug::DIAGNOSTICS) {
        diagnostics.EndLoop(now);
    }
}
