#pragma once

#include "Unit.h"

static const float FIRE_BALL_DURATION = .5f;
static const Vector2 FIRE_BALL_START_OFFSET = {2, 0};
static const float FIRE_BACLL_START_HEIGHT = 4.f;

class FallingFireBall : public Unit
{
public:
    virtual void onSpawn() override;
    virtual void render() override;
    virtual void rts_update() override;

    Vector2 fromPos;
    Vector2 toPos;
    float angle;
    float fallingProgress = 0;
};
