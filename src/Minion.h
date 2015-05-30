#pragma once

#include "Globals.h"

class Minion : public Unit
{
public:
    virtual void rts_update();
    
    void idle();
    void walkTo(Unit *in_pWaypoint);
    void walkTo(const Vector2 &in_targetPos);
    void restartAnim();
    void updateDirection();

    enum class eState
    {
        IDLE,
        WALK_TO,
        WALK_TO_TARGET,
        ATTACK
    } state = eState::IDLE;

    sAnim anim;
    Vector2 targetPos;
    int direction = 0;
    Unit *pWaypoint = nullptr;
};
