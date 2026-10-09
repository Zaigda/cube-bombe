#pragma once
#include "Enigma.h"
#include "Board1/Outputs/Screen.h"
#include "Common/Inputs/Button.h"

// A shot going up; y is its top LED.
struct Bullet {
    int x;
    int y;
    bool active;
};

// Enigma 2: the singer moves the ship by voice, only the guide sees the aliens. A landed alien is a mistake.
class InvadersEnigma : public Enigma {
public:
    InvadersEnigma(const Button &leftButton, const Button &rightButton, Screen &guideScreen, Screen &singerScreen);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

private:
    void RestartWave();

    void MoveShip(uint32_t now);

    void Fire(uint32_t now);

    void MoveBullets(uint32_t now);

    void MoveAlien(uint32_t now);

    void CheckHits(uint32_t now);

    void SpawnAlien(uint32_t now);

    bool HasAlienLanded() const;

    void DrawKillCounter(Screen &screen, uint16_t color) const;

    void DrawShip(Screen &screen) const;

    int ShipY() const;

    static constexpr int MAX_BULLETS = 4;

    const Button &moveLeftButton;
    const Button &moveRightButton;
    Screen &guide;
    Screen &singer;

    int shipX = 0;
    uint32_t lastShipMoveMs = 0;
    Bullet bullets[MAX_BULLETS] = {};
    uint32_t lastFireMs = 0;
    uint32_t lastBulletMoveMs = 0;
    int alienX = 0;
    int alienY = 0;
    uint32_t lastAlienMoveMs = 0;
    bool alienWaiting = true;
    int aliensKilled = 0;
    uint32_t lastHitMs = 0;
    int hitX = 0;
    int hitY = 0;
};
