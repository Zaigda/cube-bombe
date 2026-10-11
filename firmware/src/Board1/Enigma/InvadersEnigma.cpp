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
    lastFireMs = now;
    lastBulletMoveMs = now;
    RestartWave();
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

    if (HasAlienLanded()) {
        Serial.println("Alien landed: kill counter back to 0");
        RestartWave();
        return EnigmaEvent::Mistake;
    }
    if (aliensKilled >= InvadersTuning::ALIEN_COUNT) {
        return EnigmaEvent::Solved;
    }

    Fire(now);
    return EnigmaEvent::None;
}

void InvadersEnigma::Draw(uint32_t now) {
    const bool hitFlash = lastHitMs != 0 && now - lastHitMs < InvadersTuning::HIT_FLASH_MS;
    if (!alienWaiting) {
        guide.drawBitmap(alienX, alienY, ALIEN_SPRITE, InvadersTuning::ALIEN_SIZE, InvadersTuning::ALIEN_SIZE,
                         Palette::GREEN);
    }
    if (hitFlash) {
        guide.drawBitmap(hitX, hitY, ALIEN_SPRITE, InvadersTuning::ALIEN_SIZE, InvadersTuning::ALIEN_SIZE,
                         Palette::ORANGE);
    }
    for (const Bullet &bullet : bullets) {
        if (bullet.active) {
            guide.fillRect(bullet.x, bullet.y, 1, InvadersTuning::BULLET_HEIGHT, Palette::YELLOW);
        }
    }
    DrawShip(guide);
    DrawShip(singer);
    if (!alienWaiting) {
        const uint8_t level = InvadersTuning::COUNTER_LEVEL;
        DrawKillCounter(guide, hitFlash ? Palette::WHITE : Palette::Rgb(level, level, level));
    }
}

void InvadersEnigma::RestartWave() {
    for (Bullet &bullet : bullets) {
        bullet.active = false;
    }
    aliensKilled = 0;
    alienWaiting = true;
    lastHitMs = 0;
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
    const int nextX = shipX + (left ? -1 : 1);
    if (InvadersTuning::WRAP_AROUND && nextX < minX) {
        shipX = maxX;
    } else if (InvadersTuning::WRAP_AROUND && nextX > maxX) {
        shipX = minX;
    } else {
        shipX = constrain(nextX, minX, maxX);
    }
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

void InvadersEnigma::MoveAlien(uint32_t now) {
    if (now - lastAlienMoveMs < InvadersTuning::ALIEN_STEP_MS) {
        return;
    }
    lastAlienMoveMs = now;
    alienY++;
}

void InvadersEnigma::CheckHits(uint32_t now) {
    for (Bullet &bullet : bullets) {
        const bool insideX = bullet.x >= alienX && bullet.x < alienX + InvadersTuning::ALIEN_SIZE;
        const bool insideY = bullet.y < alienY + InvadersTuning::ALIEN_SIZE;
        if (bullet.active && insideX && insideY) {
            bullet.active = false;
            aliensKilled++;
            Serial.printf("Alien hit: %d killed\n", aliensKilled);
            hitX = alienX;
            hitY = alienY;
            lastHitMs = now;
            SpawnAlien(now);
            return;
        }
    }
}

void InvadersEnigma::SpawnAlien(uint32_t now) {
    const int column = random(InvadersTuning::COLUMN_COUNT);
    alienX = InvadersTuning::FIRST_COLUMN_X + column * InvadersTuning::ALIEN_SIZE;
    alienY = 0;
    lastAlienMoveMs = now;
}

bool InvadersEnigma::HasAlienLanded() const {
    return alienY + InvadersTuning::ALIEN_SIZE > ShipY();
}

void InvadersEnigma::DrawKillCounter(Screen &screen, uint16_t color) const {
    screen.setTextColor(color);
    screen.setCursor(InvadersTuning::COUNTER_LEFT, InvadersTuning::COUNTER_TOP);
    screen.print(aliensKilled);
    screen.print('/');
    screen.print(InvadersTuning::ALIEN_COUNT);
}

void InvadersEnigma::DrawShip(Screen &screen) const {
    screen.fillRect(shipX, ShipY(), InvadersTuning::SHIP_WIDTH, InvadersTuning::SHIP_HEIGHT, Palette::WHITE);
}

int InvadersEnigma::ShipY() const {
    return guide.height() - InvadersTuning::SHIP_HEIGHT;
}
