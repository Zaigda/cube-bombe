#pragma once
#include "Enigma.h"
#include "Common/Inputs/Button.h"
#include "Common/Outputs/Screen.h"

// A shot going up; y is its top LED.
struct Bullet {
    int x;
    int y;
    bool active;
};

// Enigma 2: the singer moves a ship by shouting left or right; only the guide sees the aliens.
class InvadersEnigma : public Enigma {
public:
    InvadersEnigma(const Button &leftButton, const Button &rightButton, Screen &guideScreen, Screen &singerScreen);

    void Start(uint32_t now) override;

    EnigmaEvent Update(uint32_t now) override;

    void Draw(uint32_t now) override;

    uint16_t GetSoundHz(uint32_t now) const;

private:
    void MoveShip(uint32_t now);

    void Fire(uint32_t now);

    void MoveBullets(uint32_t now);

    void MoveAlien(uint32_t now);

    void ReplayNote(uint32_t now);

    void CheckHits(uint32_t now);

    void SpawnAlien(uint32_t now);

    bool IsAlienVisible(uint32_t now) const;

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
    uint32_t alienSpawnMs = 0;
    bool alienWaiting = true;
    int alienColumn = 0;
    uint32_t lastNoteMs = 0;
    uint32_t nextReplayMs = 0;
    int aliensKilled = 0;
    int lives = 0;
};
