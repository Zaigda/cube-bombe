#include <Arduino.h>
#include "Config.h"
#include "Core/Countdown.h"

Countdown countdown;

void setup() {
    Serial.begin(115200);
    delay(2000);   // time to open the serial monitor
    Serial.println("Bomb cube - step 1: countdown");

    countdown.Start(Timing::GAME_DURATION_MS, millis());
}

void loop() {
    const uint32_t now = millis();

    // Temporary test: typing 'p' in the monitor costs a penalty.
    if (Serial.available() > 0 && Serial.read() == 'p') {
        countdown.AddPenalty(Timing::MISTAKE_PENALTY_MS);
        Serial.println("Penalty!");
    }

    // Print the remaining time once per second, not on every loop.
    static uint32_t lastPrintMs = 0;
    if (now - lastPrintMs >= 1000) {
        lastPrintMs = now;
        const unsigned int remainingS = countdown.GetRemainingMs(now) / 1000;
        Serial.printf("%u:%02u\n", remainingS / 60, remainingS % 60);
    }

    if (countdown.IsExpired(now)) {
        Serial.println("BOOM");
        countdown.Stop(now);   // stops the timer, so BOOM prints only once
    }
}