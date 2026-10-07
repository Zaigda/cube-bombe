#include "GameManager.h"
#include <Arduino.h>
#include <cstdio>
#include "Config.h"
#include "Core/Palette.h"

static constexpr uint16_t TIMER_COLOR = Palette::Rgb(0, 120, 255);

static const char *StateName(GameState state) {
    switch (state) {
        case GameState::Waiting:
            return "Waiting";
        case GameState::Playing:
            return "Playing";
        case GameState::Defused:
            return "Defused";
        case GameState::Exploded:
        default:
            return "Exploded";
    }
}

static void PrintCentered(Screen &screen, const char *text, int16_t y, uint8_t size, uint16_t color) {
    int16_t left = 0;
    int16_t top = 0;
    uint16_t textWidth = 0;
    uint16_t textHeight = 0;
    screen.setTextSize(size);
    screen.getTextBounds(text, 0, 0, &left, &top, &textWidth, &textHeight);
    screen.setCursor((screen.width() - textWidth) / 2, y);
    screen.setTextColor(color);
    screen.print(text);
}

static void DrawTimer(Screen &screen, uint32_t remainingMs) {
    const uint32_t totalSeconds = (remainingMs + 999) / 1000;
    const unsigned minutes = totalSeconds / 60;
    const unsigned seconds = totalSeconds % 60;
    char text[12];
    snprintf(text, sizeof(text), "%u:%02u", minutes, seconds);
    PrintCentered(screen, text, 0, 1, TIMER_COLOR);
}

GameManager::GameManager(Countdown &gameCountdown, const Button &startButton, BombCode &bombCode)
    : countdown(gameCountdown), start(startButton), code(bombCode) {
}

void GameManager::AddEnigma(Enigma &enigma) {
    if (enigmaCount >= MAX_ENIGMAS) {
        Serial.println("GameManager: too many enigmas, increase MAX_ENIGMAS");
        return;
    }
    enigmas[enigmaCount] = &enigma;
    enigmaCount++;
}

void GameManager::AddScreen(Screen &screen) {
    if (screenCount >= MAX_SCREENS) {
        Serial.println("GameManager: too many screens, increase MAX_SCREENS");
        return;
    }
    screens[screenCount] = &screen;
    screenCount++;
}

void GameManager::Update(uint32_t now) {
    switch (state) {
        case GameState::Waiting:
            if (start.WasPressed()) {
                StartGame(now);
            }
            break;
        case GameState::Playing:
            UpdatePlaying(now);
            break;
        case GameState::Defused:
        case GameState::Exploded:
            if (now - stateStartMs >= Timing::RESULT_SCREEN_MS) {
                ChangeState(GameState::Waiting, now);
            }
            break;
    }
}

void GameManager::Draw(uint32_t now) {
    const uint32_t remainingMs = countdown.GetRemainingMs(now);
    const bool blinkOn = ((now - stateStartMs) / BLINK_MS) % 2 == 0;

    for (uint8_t i = 0; i < screenCount; i++) {
        Screen &screen = *screens[i];
        const int16_t middle = screen.height() / 2;
        screen.fillScreen(Palette::BLACK);

        switch (state) {
            case GameState::Waiting:
                PrintCentered(screen, "PRESS", middle - 9, 1, Palette::WHITE);
                PrintCentered(screen, "START", middle + 1, 1, Palette::WHITE);
                break;
            case GameState::Playing:
                DrawTimer(screen, remainingMs);
                break;
            case GameState::Defused:
                DrawTimer(screen, remainingMs);
                PrintCentered(screen, "OK", middle - 8, 2, Palette::GREEN);
                break;
            case GameState::Exploded:
                if (blinkOn) {
                    PrintCentered(screen, "BOOM", middle - 8, 2, Palette::RED);
                }
                break;
        }
    }

    if (state == GameState::Playing) {
        enigmas[currentEnigma]->Draw(now);
    }
}

GameState GameManager::GetState() const {
    return state;
}

void GameManager::StartGame(uint32_t now) {
    if (enigmaCount == 0) {
        Serial.println("GameManager: no enigma to play, call AddEnigma in setup");
        return;
    }
    code.Shuffle();
    countdown.Start(Timing::GAME_DURATION_MS, now);
    currentEnigma = 0;
    enigmas[currentEnigma]->Start(now);
    ChangeState(GameState::Playing, now);
}

void GameManager::UpdatePlaying(uint32_t now) {
    const EnigmaEvent event = enigmas[currentEnigma]->Update(now);

    if (event == EnigmaEvent::Mistake) {
        countdown.AddPenalty(Timing::MISTAKE_PENALTY_MS);
        Serial.println("Game: mistake, time penalty");
    } else if (event == EnigmaEvent::Solved) {
        currentEnigma++;
        if (currentEnigma == enigmaCount) {
            countdown.Stop(now);
            ChangeState(GameState::Defused, now);
            return;
        }
        enigmas[currentEnigma]->Start(now);
        Serial.printf("Game: enigma %u of %u\n", currentEnigma + 1, enigmaCount);
    }

    if (countdown.IsExpired(now)) {
        countdown.Stop(now);
        ChangeState(GameState::Exploded, now);
    }
}

void GameManager::ChangeState(GameState newState, uint32_t now) {
    state = newState;
    stateStartMs = now;
    Serial.printf("Game: %s\n", StateName(newState));
}
