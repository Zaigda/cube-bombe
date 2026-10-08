#include <Arduino.h>

#define UART_RX 1
#define UART_TX 44

void setup()
{
    Serial.begin(115200);

    Serial2.begin(115200, SERIAL_8N1, UART_RX, UART_TX);

    Serial.println("ESP32 B prêt !");
}

void loop()
{
    // Vérifier si A a envoyé un message
    if (Serial2.available())
    {
        String message = Serial2.readStringUntil('\n');

        Serial.print("A -> B : ");
        Serial.println(message);

        // Répondre à l'ESP32 A
        Serial2.println("PONG");

        Serial.println("B -> A : PONG");
    }
}