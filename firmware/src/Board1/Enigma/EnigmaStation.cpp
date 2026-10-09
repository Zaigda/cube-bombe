#include "EnigmaStation.h"
#include <Arduino.h>
#include <cstdio>
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

static uint16_t GetPulseColor(uint32_t now) {
    const uint32_t halfMs = GameTuning::PULSE_MS / 2;
    const uint32_t cycleMs = now % GameTuning::PULSE_MS;
    const uint32_t risingMs = cycleMs < halfMs ? cycleMs : GameTuning::PULSE_MS - cycleMs;
    const uint32_t extraLevel = (255 - GameTuning::PULSE_MIN_LEVEL) * risingMs / halfMs;
    const uint8_t level = static_cast<uint8_t>(GameTuning::PULSE_MIN_LEVEL + extraLevel);
    return Palette::Rgb(level, level, level);
}

EnigmaStation::EnigmaStation(const char *stationName, Enigma &stationEnigma, const Button &startButton,
                             const Countdown &bombTimer)
    : name(stationName), enigma(stationEnigma), start(startButton), timer(bombTimer) {
}

void EnigmaStation::AddScreen(Screen &screen) {
    if (screenCount >= MAX_SCREENS) {
        Serial.printf("%s: too many screens, increase MAX_SCREENS\n", name);
        return;
    }
    screens[screenCount] = &screen;
    screenCount++;
}

void EnigmaStation::Update(uint32_t now) {
    if (!frozen) {
        ReadStartButton(now);
        UpdateEnigma(now);
    }
}

void EnigmaStation::Draw(uint32_t now) {
    if (phase == Phase::Playing) {
        enigma.Draw(now);
    }
    for (uint8_t i = 0; i < screenCount; i++) {
        DrawTexts(*screens[i], now);
    }
}

void EnigmaStation::Reset(uint32_t now) {
    enigma.Start(now);
    ledState = LedState::Off;
    phase = Phase::Paused;
    frozen = false;
}

void EnigmaStation::Freeze(LedState endLed) {
    ledState = endLed;
    frozen = true;
}

bool EnigmaStation::IsStarted() const {
    return phase != Phase::Paused;
}

bool EnigmaStation::IsPlaying() const {
    return phase == Phase::Playing;
}

bool EnigmaStation::IsSolved() const {
    return phase == Phase::Solved;
}

LedState EnigmaStation::GetLedState() const {
    return ledState;
}

void EnigmaStation::ReadStartButton(uint32_t now) {
    if (!start.WasPressed() || phase == Phase::Solved) {
        return;
    }
    if (phase == Phase::Paused) {
        Serial.printf("%s: start\n", name);
        ledState = LedState::Blinking;
        phase = Phase::Playing;
    } else {
        Serial.printf("%s: reset\n", name);
        Reset(now);
    }
}

void EnigmaStation::UpdateEnigma(uint32_t now) {
    if (phase == Phase::Playing) {
        const EnigmaEvent event = enigma.Update(now);
        if (event == EnigmaEvent::Mistake) {
            Serial.printf("%s: mistake\n", name);
            failedMs = now;
            phase = Phase::Failed;
        } else if (event == EnigmaEvent::Solved) {
            Serial.printf("%s: solved!\n", name);
            ledState = LedState::On;
            solvedMs = now;
            phase = Phase::Solved;
        }
    } else if (phase == Phase::Failed && now - failedMs >= StationTuning::FAIL_MS) {
        phase = Phase::Playing;
    }
}

void EnigmaStation::DrawTexts(Screen &screen, uint32_t now) const {
    switch (phase) {
        case Phase::Paused:
            screen.DrawCenteredText(Texts::START, start.IsDown() ? Palette::GREEN : GetPulseColor(now),
                                    StationTuning::START_TEXT_TOP);
            DrawTimer(screen, now);
            break;
        case Phase::Playing:
            break;
        case Phase::Failed:
            screen.fillScreen(Palette::RED);
            screen.setTextSize(StationTuning::FAILED_TEXT_SIZE);
            screen.DrawCenteredText(Texts::FAILED, Palette::WHITE);
            screen.setTextSize(1);
            break;
        case Phase::Solved:
            if (now - solvedMs < StationTuning::SOLVED_FLASH_MS) {
                screen.fillScreen(Palette::GREEN);
                screen.DrawCenteredText(Texts::SOLVED, Palette::BLACK);
            } else {
                screen.DrawCenteredText(Texts::SOLVED, Palette::GREEN);
            }
            break;
    }
}

void EnigmaStation::DrawTimer(Screen &screen, uint32_t now) const {
    const uint32_t remainingMs = timer.GetRemainingMs(now);
    const unsigned totalSeconds = (remainingMs + 999) / 1000;
    char text[12];
    snprintf(text, sizeof(text), "%u:%02u", totalSeconds / 60, totalSeconds % 60);

    const uint16_t color = remainingMs < GameTuning::FINAL_WARNING_MS ? Palette::RED : StationTuning::TIMER_COLOR;
    screen.setTextSize(StationTuning::TIMER_TEXT_SIZE);
    screen.DrawCenteredText(text, color, StationTuning::TIMER_TEXT_TOP);
    screen.setTextSize(1);
}
