#include "InvadersEnigma.h"
#include <Arduino.h>
#include "Board1/Config.h"
#include "Board1/Core/Palette.h"

static const uint8_t ALIEN_SPRITE[] = {
    0b00011000,
    0b00111100,
    0b01111110,
    0b11011011,
    0b11111111,
    0b00100100,
    0b01011010,
    0b10100101,
};

InvadersEnigma::InvadersEnigma(const Button &leftButton, const Button &rightButton, Screen &guideScreen,
                               Screen &singerScreen)
    : moveLeftButton(leftButton), moveRightButton(rightButton), guide(guideScreen), singer(singerScreen) {
}

void InvadersEnigma::Start(uint32_t now) {
    shipX = (guide.width() - InvadersTuning::SHIP_WIDTH) / 2;
    lastShipMoveMs = now;
    for (Bullet &bullet : bullets) {
        bullet.active = false;
    }
    lastFireMs = now;
    lastBulletMoveMs = now;
    aliensKilled = 0;
    lives = InvadersTuning::START_LIVES;
    alienWaiting = true;
}

EnigmaEvent InvadersEnigma::Update(uint32_t now) {
    if (alienWaiting) {
        alienWaiting = false;
        SpawnAlien(now);
    }
    MoveShip(now);
    MoveBullets(now);
    MoveAlien(now);
    CheckHits(now);
    ReplayNote(now);

    if (lives <= 0) {
        Start(now);
        return EnigmaEvent::Mistake;
    }
    if (aliensKilled >= InvadersTuning::ALIEN_COUNT) {
        return EnigmaEvent::Solved;
    }

    Fire(now);
    return EnigmaEvent::None;
}

void InvadersEnigma::Draw(uint32_t now) {
    if (!alienWaiting && IsAlienVisible(now)) {
        guide.drawBitmap(alienX, alienY, ALIEN_SPRITE, InvadersTuning::ALIEN_SIZE, InvadersTuning::ALIEN_SIZE,
                         Palette::GREEN);
    }
    for (int i = 0; i < lives; i++) {
        const int size = InvadersTuning::LIFE_SIZE;
        guide.fillRect(1, 1 + i * (size + 1), size, size, Palette::RED);
    }
    for (const Bullet &bullet : bullets) {
        if (bullet.active) {
            guide.fillRect(bullet.x, bullet.y, 1, InvadersTuning::BULLET_HEIGHT, Palette::YELLOW);
        }
    }
    DrawShip(guide);
    DrawShip(singer);
}

void InvadersEnigma::MoveShip(uint32_t now) {
    if (now - lastShipMoveMs < InvadersTuning::SHIP_STEP_MS) {
        return;
    }
    lastShipMoveMs = now;

    const bool left = moveLeftButton.IsDown();
    const bool right = moveRightButton.IsDown();
    if (left == right) {
        return;
    }

    const int minX = InvadersTuning::SIDE_MARGIN;
    const int maxX = guide.width() - InvadersTuning::SIDE_MARGIN - InvadersTuning::SHIP_WIDTH;
    shipX = constrain(shipX + (left ? -1 : 1), minX, maxX);
}

void InvadersEnigma::Fire(uint32_t now) {
    if (now - lastFireMs < InvadersTuning::FIRE_MS) {
        return;
    }
    lastFireMs = now;

    for (Bullet &bullet : bullets) {
        if (!bullet.active) {
            bullet = {shipX + InvadersTuning::SHIP_WIDTH / 2, ShipY() - InvadersTuning::BULLET_HEIGHT, true};
            return;
        }
    }
}

void InvadersEnigma::MoveBullets(uint32_t now) {
    if (now - lastBulletMoveMs < InvadersTuning::BULLET_STEP_MS) {
        return;
    }
    lastBulletMoveMs = now;

    for (Bullet &bullet : bullets) {
        if (!bullet.active) {
            continue;
        }
        bullet.y--;
        if (bullet.y + InvadersTuning::BULLET_HEIGHT <= 0) {
            bullet.active = false;
        }
    }
}

void InvadersEnigma::DrawShip(Screen &screen) const {
    screen.fillRect(shipX, ShipY(), InvadersTuning::SHIP_WIDTH, InvadersTuning::SHIP_HEIGHT, Palette::WHITE);
}

int InvadersEnigma::ShipY() const {
    return guide.height() - InvadersTuning::SHIP_HEIGHT;
}

void InvadersEnigma::MoveAlien(uint32_t now) {
    if (now - lastAlienMoveMs < InvadersTuning::ALIEN_STEP_MS) {
        return;
    }
    lastAlienMoveMs = now;

    alienY++;
    if (alienY + InvadersTuning::ALIEN_SIZE > ShipY()) {
        lives--;
        Serial.printf("Alien landed: %d lives left\n", lives);
        SpawnAlien(now);
    }
}

void InvadersEnigma::CheckHits(uint32_t now) {
    for (Bullet &bullet : bullets) {
        const bool insideX = bullet.x >= alienX && bullet.x < alienX + InvadersTuning::ALIEN_SIZE;
        const bool insideY = bullet.y < alienY + InvadersTuning::ALIEN_SIZE;
        if (bullet.active && insideX && insideY) {
            bullet.active = false;
            aliensKilled++;
            Serial.printf("Alien hit: %d killed\n", aliensKilled);
            SpawnAlien(now);
            return;
        }
    }
}

void InvadersEnigma::SpawnAlien(uint32_t now) {
    alienColumn = random(InvadersTuning::COLUMN_COUNT);
    alienX = InvadersTuning::FIRST_COLUMN_X + alienColumn * InvadersTuning::ALIEN_SIZE;
    alienY = 0;
    lastAlienMoveMs = now;
    alienSpawnMs = now;
    lastNoteMs = now;
    nextReplayMs = now + random(InvadersTuning::REPLAY_MIN_MS, InvadersTuning::REPLAY_MAX_MS);
}

bool InvadersEnigma::IsAlienVisible(uint32_t now) const {
    const int alienNumber = aliensKilled + 1;
    return alienNumber < InvadersTuning::INVISIBLE_FROM || now - alienSpawnMs < InvadersTuning::INVISIBLE_AFTER_MS;
}

uint16_t InvadersEnigma::GetSoundHz(uint32_t now) const {
    return now - lastNoteMs < InvadersTuning::NOTE_MS ? InvadersTuning::NOTES_HZ[alienColumn] : 0;
}

void InvadersEnigma::ReplayNote(uint32_t now) {
    if (IsAlienVisible(now) || static_cast<int32_t>(now - nextReplayMs) < 0) {
        return;
    }
    lastNoteMs = now;
    nextReplayMs = now + random(InvadersTuning::REPLAY_MIN_MS, InvadersTuning::REPLAY_MAX_MS);
}
