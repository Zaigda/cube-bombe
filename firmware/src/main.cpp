#include <Arduino.h>
#include "Config.h"
#include "Core/BombCode.h"
#include "Core/Palette.h"
#include "Enigma/Enigma.h"
#include "Enigma/SnakeEnigma.h"
#include "Inputs/EitherButton.h"
#include "Inputs/InputManager.h"
#include "Inputs/PinButton.h"
#include "Inputs/SerialKeyboard.h"
#include "Outputs/PanelOutput.h"
#include "Outputs/Screen.h"

SerialKeyboard keyboard(Serial);
PinButton leftPin(Pins::SNAKE_LEFT_BUTTON);
PinButton rightPin(Pins::SNAKE_RIGHT_BUTTON);
EitherButton leftButton(leftPin, keyboard.Left());
EitherButton rightButton(rightPin, keyboard.Right());
InputManager inputs;

Screen snakeScreen(64, 64, 0);
PanelOutput output(Pins::HUB75, 2);

BombCode code;
SnakeEnigma snake(leftButton, rightButton, snakeScreen, code);

namespace {
    constexpr uint32_t TEST_STEP_MS = 1000;
    constexpr uint8_t TEST_STEPS = 6;
    bool displayTestActive = false;
    uint32_t displayTestStartedMs = 0;
    int lastTestStep = -1;

    void DrawDisplayTest(uint8_t step) {
        static const uint16_t colors[] = {
            Palette::BLACK, Palette::RED, Palette::GREEN, Palette::BLUE
        };
        static const char *labels[] = {
            "black", "red", "green", "blue", "top half white", "bottom half white"
        };
        if (step != lastTestStep) {
            Serial.printf("Display test: %s\n", labels[step]);
            lastTestStep = step;
        }
        snakeScreen.fillScreen(step < 4 ? colors[step] : Palette::BLACK);
        if (step >= 4) {
            const int halfHeight = snakeScreen.height() / 2;
            snakeScreen.fillRect(0, step == 4 ? 0 : halfHeight,
                                 snakeScreen.width(), halfHeight, Palette::WHITE);
        }
        output.Show();
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
    Serial.println("Bomb cube - snake");
    Serial.println("USB keyboard: Q = turn left, D = turn right, T = display test.");
    Serial.println("Use a 115200 baud serial terminal that sends keys immediately.");

    inputs.Add(keyboard);
    inputs.Add(leftPin);
    inputs.Add(rightPin);
    inputs.Begin();

    code.Shuffle();
    snake.Start(millis());
}

void loop() {
    const uint32_t now = millis();

    inputs.Update(now);

    if (!displayTestActive && keyboard.TestRequested()) {
        displayTestActive = true;
        displayTestStartedMs = now;
        lastTestStep = -1;
    }
    if (displayTestActive) {
        const uint32_t step = (now - displayTestStartedMs) / TEST_STEP_MS;
        if (step < TEST_STEPS) {
            DrawDisplayTest(static_cast<uint8_t>(step));
            return;
        }
        displayTestActive = false;
        code.Shuffle();
        snake.Start(now);
        Serial.println("Display test complete. New game.");
        // Commands read during the test must not affect the new game.
        snakeScreen.fillScreen(Palette::BLACK);
        snake.Draw(now);
        output.Show();
        return;
    }

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
