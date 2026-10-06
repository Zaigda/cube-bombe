#include <Arduino.h>
#include "Config.h"
#include "Core/Countdown.h"
#include "Core/Palette.h"
#include "Outputs/Screen.h"
#include "Outputs/SerialOutput.h"

Countdown countdown;
Screen testScreen(64, 64, 0);
SerialOutput output;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Bomb cube - step 2b: serial output");

    output.AddScreen(testScreen);
    if (!output.Begin()) {
        Serial.println("Output failed to start");
    }


    testScreen.fillRect(10, 10, 4, 4, Palette::RED);
    testScreen.drawLine(0, 63, 63, 30, Palette::GREEN);
    testScreen.setTextSize(2);
    testScreen.setCursor(20, 40);
    testScreen.print("HI");

    countdown.Start(Timing::GAME_DURATION_MS, millis());
}

void loop() {
    const uint32_t now = millis();

    if (Serial.available() > 0 && Serial.read() == 'p') {
        countdown.AddPenalty(Timing::MISTAKE_PENALTY_MS);
        Serial.println("Penalty!");
    }

    static uint32_t lastPrintMs = 0;
    if (now - lastPrintMs >= 1000) {
        lastPrintMs = now;
        const unsigned int remainingS = countdown.GetRemainingMs(now) / 1000;
        Serial.printf("%u:%02u\n", remainingS / 60, remainingS % 60);
    }

    if (countdown.IsExpired(now)) {
        Serial.println("BOOM");
        countdown.Stop(now);
    }

    output.Show();
}
