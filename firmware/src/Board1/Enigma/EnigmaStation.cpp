#include "EnigmaStation.h"
#include <Arduino.h>
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

uint8_t EnigmaStation::stationTotal = 0;
uint8_t EnigmaStation::solvedTotal = 0;

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
    stationTotal++;
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
    if (phase == Phase::Solved) {
        solvedTotal--;
    }
    enigma.Reset(now);
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

bool EnigmaStation::IsInFinalAlarm(uint32_t now) const {
    const bool notInPlay = phase == Phase::Paused || phase == Phase::Solved;
    return notInPlay && timer.IsRunning() && timer.GetRemainingMs(now) <= GameTuning::FINAL_ALARM_MS;
}

bool EnigmaStation::IsFinalAlarmLit(uint32_t now) {
    return (now / GameTuning::FINAL_ALARM_BLINK_MS) % 2 == 0;
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
            solvedTotal++;
        }
    } else if (phase == Phase::Failed && now - failedMs >= StationTuning::FAIL_MS) {
        phase = Phase::Playing;
    }
}

void EnigmaStation::DrawTexts(Screen &screen, uint32_t now) const {
    switch (phase) {
        case Phase::Paused: {
            const uint16_t color = start.IsDown() ? Palette::GREEN : GetPulseColor(now);
            screen.DrawCenteredText(Texts::WAITING, color, StationTuning::WAITING_TEXT_TOP);
            DrawTimer(screen, now);
            DrawProgress(screen, Palette::GREEN, StationTuning::PROGRESS_TEXT_TOP);
            break;
        }
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
                DrawSolved(screen, Palette::BLACK);
            } else {
                DrawSolved(screen, Palette::GREEN);
            }
            break;
    }
    DrawFinalAlarm(screen, now);
}

void EnigmaStation::DrawTimer(Screen &screen, uint32_t now) const {
    const uint32_t remainingMs = timer.GetRemainingMs(now);
    const uint16_t color = remainingMs < GameTuning::FINAL_WARNING_MS ? Palette::RED : StationTuning::TIMER_COLOR;
    screen.setTextSize(StationTuning::TIMER_TEXT_SIZE);
    screen.DrawCenteredTime(remainingMs, color, StationTuning::WAITING_TIMER_TOP);
    screen.setTextSize(1);
}

void EnigmaStation::DrawSolved(Screen &screen, uint16_t color) const {
    const int16_t top = (screen.height() - 3 * StationTuning::TEXT_LINE) / 2;
    screen.DrawCenteredText(Texts::SOLVED, color, top);
    screen.DrawCenteredText(Texts::SOLVED_LINE_2, color, top + StationTuning::TEXT_LINE);
    DrawProgress(screen, color, top + 2 * StationTuning::TEXT_LINE);
}

void EnigmaStation::DrawProgress(Screen &screen, uint16_t color, int16_t top) {
    char progress[8];
    snprintf(progress, sizeof(progress), "%u/%u", solvedTotal, stationTotal);
    screen.DrawCenteredText(progress, color, top);
}

void EnigmaStation::DrawFinalAlarm(Screen &screen, uint32_t now) const {
    if (!IsInFinalAlarm(now) || !IsFinalAlarmLit(now)) {
        return;
    }
    for (int16_t i = 0; i < GameTuning::FINAL_ALARM_BORDER; i++) {
        screen.drawRect(i, i, screen.width() - 2 * i, screen.height() - 2 * i, Palette::RED);
    }
}
