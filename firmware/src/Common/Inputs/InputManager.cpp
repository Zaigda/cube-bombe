#include "InputManager.h"
#include <Arduino.h>

void InputManager::Add(Sensor &sensor) {
    if (sensorCount >= MAX_SENSORS) {
        Serial.println("InputManager: too many sensors, increase MAX_SENSORS");
        return;
    }
    sensors[sensorCount] = &sensor;
    sensorCount++;
}

void InputManager::Begin() {
    for (uint8_t i = 0; i < sensorCount; i++) {
        sensors[i]->Begin();
    }
}

void InputManager::Update(uint32_t now) {
    for (uint8_t i = 0; i < sensorCount; i++) {
        sensors[i]->Update(now);
    }
}
