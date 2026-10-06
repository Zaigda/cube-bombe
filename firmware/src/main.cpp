#include <Arduino.h>
#include "Config.h"
#include "Core/Countdown.h"
#include "Core/Palette.h"
#include "Outputs/Screen.h"
#include "Outputs/SerialOutput.h"

constexpr int16_t CELL_SIZE = 4;     // one game cell = 4x4 pixels
constexpr int16_t SQUARE_ROW = 10;   // the row of cells the square moves along
constexpr uint32_t STEP_MS = 200;    // the square moves one cell every 200 ms

Countdown countdown;
Screen testScreen(64, 64, 0);   // temporary: step 2 test
SerialOutput output;

int16_t squareColumn = 0;   // position of the square, in cells
uint32_t lastStepMs = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Bomb cube - step 2c: moving square");

    output.AddScreen(testScreen);
    if (!output.Begin()) {
        Serial.println("Output failed to start");
    }

    countdown.Start(Timing::GAME_DURATION_MS, millis());
}

void loop() {
    const uint32_t now = millis();

    // Temporary test: typing 'p' in the monitor costs a penalty.
    if (Serial.available() > 0 && Serial.read() == 'p') {
        countdown.AddPenalty(Timing::MISTAKE_PENALTY_MS);
    }

    // 1. Logic: one cell to the right every STEP_MS, back to the left after the edge.
    if (now - lastStepMs >= STEP_MS) {
        lastStepMs = now;
        const int16_t columns = testScreen.width() / CELL_SIZE;
        squareColumn = (squareColumn + 1) % columns;
    }

    // 2. Drawing: start from a black image and redraw everything.
    testScreen.fillScreen(Palette::BLACK);
    testScreen.fillRect(squareColumn * CELL_SIZE, SQUARE_ROW * CELL_SIZE, CELL_SIZE, CELL_SIZE, Palette::RED);

    const unsigned int remainingS = countdown.GetRemainingMs(now) / 1000;
    testScreen.setTextSize(2);
    testScreen.setCursor(8, 4);
    testScreen.printf("%u:%02u", remainingS / 60, remainingS % 60);

    // 3. Display: only prints when the image changed.
    output.Show();
}