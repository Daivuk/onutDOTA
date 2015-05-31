#pragma once

#include "Globals.h"
#include "micropather.h"

class Minion : public Unit
{
public:
    Minion();

    virtual void rts_update();
    
    void idle();
    void walkTo(Unit *in_pWaypoint);
    void walkTo(const Vector2 &in_targetPos);
    void restartAnim();
    void updateDirection();
    void attack(Unit *in_pTarget);
#if _DEBUG
    void renderDebug();
#endif

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
    int attackDelay = 0;
    uint32_t radiusCheckId = 0;
    static uint32_t s_radiusCheckId;
    uint32_t frame = 0;
    std::vector<Vector2> path;
};
