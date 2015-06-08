#pragma once

#include "Unit.h"

#define GIBS_LIFE 10

class Gibs : public Unit
{
public:
    virtual void render() override;
    virtual void rts_update() override;
    virtual void onSpawn() override;

    Vector3 velocity;
    float yOffset;
    float angle = onut::randf(0, 360);
    float angleSpeed = onut::randf(-180, 180);
    float life = GIBS_LIFE;
    int state = 0;
};
