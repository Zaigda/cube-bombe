#include "GameManager.h"
#include <Arduino.h>
#include "Board1/Config.h"

static const char *StateName(GameState state) {
    switch (state) {
        case GameState::Waiting:
            return "waiting";
        case GameState::Running:
            return "running";
        case GameState::Defused:
            return "defused";
        case GameState::Exploded:
        default:
            return "exploded";
    }
}

GameManager::GameManager(Countdown &bombTimer, const Button &globalResetButton)
    : countdown(bombTimer), resetButton(globalResetButton) {
}

void GameManager::AddStation(EnigmaStation &station) {
    if (stationCount >= MAX_STATIONS) {
        Serial.println("GameManager: too many stations, increase MAX_STATIONS");
        return;
    }
    stations[stationCount] = &station;
    stationCount++;
}

void GameManager::Begin(uint32_t now) {
    for (uint8_t i = 0; i < stationCount; i++) {
        stations[i]->Reset(now);
    }
    countdown.Reset(GameTuning::GAME_DURATION_MS);
    ChangeState(GameState::Waiting, now);
}

void GameManager::Update(uint32_t now) {
    UpdateResetHold(now);
    for (uint8_t i = 0; i < stationCount; i++) {
        stations[i]->Update(now);
    }

    if (state == GameState::Waiting && IsAnyStationStarted()) {
        countdown.Start(now);
        ChangeState(GameState::Running, now);
    }
    if (state == GameState::Running) {
        UpdateRunning(now);
    }
}

GameState GameManager::GetState() const {
    return state;
}

bool GameManager::IsOver() const {
    return state == GameState::Defused || state == GameState::Exploded;
}

uint32_t GameManager::GetTimeInStateMs(uint32_t now) const {
    return now - stateStartMs;
}

uint32_t GameManager::GetResetHoldMs(uint32_t now) const {
    if (!resetHeld) {
        return 0;
    }
    return now - resetHoldStartMs;
}

uint32_t GameManager::GetRemainingMs(uint32_t now) const {
    return countdown.GetRemainingMs(now);
}

void GameManager::UpdateResetHold(uint32_t now) {
    if (!resetButton.IsDown()) {
        resetHeld = false;
        return;
    }
    if (!resetHeld) {
        resetHeld = true;
        resetDone = false;
        resetHoldStartMs = now;
    }
    if (!resetDone && now - resetHoldStartMs >= GameTuning::GLOBAL_RESET_HOLD_MS) {
        resetDone = true;
        Reset(now);
    }
}

void GameManager::UpdateRunning(uint32_t now) {
    if (AreAllStationsSolved()) {
        End(GameState::Defused, LedState::On, now);
    } else if (countdown.IsExpired(now)) {
        End(GameState::Exploded, LedState::FastBlinking, now);
    }
}

bool GameManager::IsAnyStationStarted() const {
    for (uint8_t i = 0; i < stationCount; i++) {
        if (stations[i]->IsStarted()) {
            return true;
        }
    }
    return false;
}

bool GameManager::AreAllStationsSolved() const {
    for (uint8_t i = 0; i < stationCount; i++) {
        if (!stations[i]->IsSolved()) {
            return false;
        }
    }
    return stationCount > 0;
}

void GameManager::Reset(uint32_t now) {
    Serial.println("Game: global reset");
    for (uint8_t i = 0; i < stationCount; i++) {
        stations[i]->Reset(now);
    }
    countdown.Reset(GameTuning::GAME_DURATION_MS);
    ChangeState(GameState::Waiting, now);
}

void GameManager::End(GameState endState, LedState stationLed, uint32_t now) {
    countdown.Stop(now);
    for (uint8_t i = 0; i < stationCount; i++) {
        stations[i]->Freeze(stationLed);
    }
    ChangeState(endState, now);
}

void GameManager::ChangeState(GameState newState, uint32_t now) {
    state = newState;
    stateStartMs = now;
    Serial.printf("Game: %s\n", StateName(newState));
}
