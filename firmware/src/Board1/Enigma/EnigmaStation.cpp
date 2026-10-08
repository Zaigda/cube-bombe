#include "EnigmaStation.h"
#include <Arduino.h>
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

EnigmaStation::EnigmaStation(const char *stationName, Enigma &stationEnigma, const Button &startButton,
                             Led &statusLed, Screen &textScreen)
    : name(stationName), enigma(stationEnigma), start(startButton), led(statusLed), screen(textScreen) {
}

void EnigmaStation::Begin(uint32_t now) {
    led.Begin();
    Reset(now);
}

void EnigmaStation::Update(uint32_t now) {
    if (start.WasPressed()) {
        if (phase == Phase::Paused) {
            Serial.printf("%s: start\n", name);
            led.Set(LedState::Blinking);
            phase = Phase::Playing;
        } else {
            Serial.printf("%s: reset\n", name);
            Reset(now);
        }
    }

    if (phase == Phase::Playing) {
        const EnigmaEvent event = enigma.Update(now);
        if (event == EnigmaEvent::Mistake) {
            Serial.printf("%s: mistake\n", name);
            failedMs = now;
            phase = Phase::Failed;
        } else if (event == EnigmaEvent::Solved) {
            Serial.printf("%s: solved!\n", name);
            led.Set(LedState::On);
            phase = Phase::Solved;
        }
    } else if (phase == Phase::Failed && now - failedMs >= StationTuning::FAIL_MS) {
        phase = Phase::Playing;
    }

    led.Update(now);
}

void EnigmaStation::Draw(uint32_t now) {
    switch (phase) {
        case Phase::Paused:
            enigma.Draw(now);
            screen.DrawCenteredText("START", Palette::WHITE, StationTuning::START_TEXT_TOP);
            break;
        case Phase::Playing:
            enigma.Draw(now);
            break;
        case Phase::Failed:
            screen.fillScreen(Palette::RED);
            screen.DrawCenteredText("RATE", Palette::WHITE);
            break;
        case Phase::Solved:
            screen.DrawCenteredText("DESAMORCE", Palette::GREEN);
            break;
    }
}

void EnigmaStation::Reset(uint32_t now) {
    enigma.Start(now);
    led.Set(LedState::Off);
    phase = Phase::Paused;
}

bool EnigmaStation::IsPlaying() const {
    return phase == Phase::Playing;
}
