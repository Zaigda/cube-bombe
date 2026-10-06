#include <Arduino.h>
#include "Hal/SerialScreen.h"

static SerialScreen screen;

void setup() {
    Serial.begin(115200);

    const uint32_t start = millis();
    while (!Serial && millis() - start < 2000UL) {
        delay(10);
    }

    Serial.println("Hello, I am the bomb!");
}

void loop() {
    const uint32_t now = millis();

    // Step 2 demo: a dot crossing the screen, one cell every 300 ms.
    // The whole frame is redrawn on every pass; Show() only prints when it changed.
    const uint8_t x = static_cast<uint8_t>((now / 300UL) % screen.Width());

    screen.Clear();
    screen.SetCell(x, screen.Height() / 2, Color::Green);
    screen.Show();
}
