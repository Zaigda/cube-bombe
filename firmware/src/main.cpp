#include <Arduino.h>

constexpr uint8_t PIN_LED = 48;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    Serial.println("Hello, I am the bomb!");
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
    neopixelWrite(PIN_LED, r, g, b);
}

void turnOff() {
    setColor(0, 0, 0);
}

// Blinks the LED `count` times, then leaves it off.
void blink(int count, uint8_t r, uint8_t g, uint8_t b, int offDelay = 150) {
    for (int i = 0; i < count; i++) {
        setColor(r, g, b);
        delay(offDelay);
        turnOff();
        delay(offDelay);
    }
}

void loop() {
    Serial.println("All good");
    setColor(0, 30, 0);     // green
    delay(3000);

    Serial.println("Warning...");
    setColor(30, 10, 0);    // orange = lots of red + a little green
    delay(2000);

    Serial.println("BOOM");
    blink(5, 30, 0, 0);     // red, fast

    delay(1000);            // off (blink() leaves the LED off)
}
