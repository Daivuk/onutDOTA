#include "Minion.h"
#include "Waypoint.h"

static const float MINION_WALK_SPEED = 2.f;
static const float MINION_TARGET_TOUCH_RADIUS = 1.f;

void Minion::rts_update()
{
    // Update state
    switch (state)
    {
        case eState::WALK_TO:
        {
            Vector2 dir = targetPos - position;
            dir.Normalize();
            position += dir * MINION_WALK_SPEED * ODT;
            if (pWaypoint)
            {
                targetPos = pWaypoint->getCenter();
            }
            if (Vector2::DistanceSquared(position, targetPos) <= MINION_TARGET_TOUCH_RADIUS * MINION_TARGET_TOUCH_RADIUS)
            {
                if (!pWaypoint)
                {
                    idle();
                }
                else
                {
                    auto pNextWaypoint = dynamic_cast<Waypoint*>(pWaypoint);
                    if (pNextWaypoint)
                    {
                        walkTo(pNextWaypoint->pNext[team]);
                    }
                    else
                    {
                        pWaypoint = nullptr;
                        idle();
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    updateDirection();

    // Update animation
    anim.progress += ODT * (float)anim.pAnimRes->pAnimDef->fps;
    anim.frame = (int)anim.progress % anim.pAnimRes->pAnimDef->frames.size();
    auto frame = anim.pAnimRes->frames[anim.frame];
    UVs = frame.UVs;
    spriteOffsetAndSize = {frame.offset, frame.size};
}

void Minion::walkTo(Unit *in_pWaypoint)
{
    pWaypoint = in_pWaypoint;
    if (pWaypoint)
    {
        walkTo(pWaypoint->getCenter());
    }
}

void Minion::walkTo(const Vector2 &in_targetPos)
{
    targetPos = in_targetPos;
    if (state == eState::WALK_TO)
    {
        return;
    }
    state = eState::WALK_TO;

    updateDirection();
    restartAnim();
}

void Minion::idle()
{
    if (state == eState::IDLE) return;
    state = eState::IDLE;
    anim.pAnimRes = &Globals::baltAnimsResources[direction | BALT_IDLE][team];
}

void Minion::updateDirection()
{
    if (state == eState::WALK_TO)
    {
        auto oldDirection = direction;
        Vector2 dir = targetPos - position;
        if (dir.y > 0.f && std::abs(dir.x) <= std::abs(dir.y))
        {
            direction = BALT_DOWN;
        }
        else if (dir.y < 0.f && std::abs(dir.x) <= std::abs(dir.y))
        {
            direction = BALT_UP;
        }
        else if (dir.x > 0.f)
        {
            direction = BALT_RIGHT;
        }
        else
        {
            direction = BALT_LEFT;
        }

        if (oldDirection != direction)
        {
            anim.pAnimRes = &Globals::baltAnimsResources[direction | BALT_WALK][team];
        }
    }
}

void Minion::restartAnim()
{
    anim.progress = 0.f;
    anim.frame = 0;
}
