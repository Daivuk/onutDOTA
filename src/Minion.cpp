#include "Minion.h"
#include "Waypoint.h"

static const float MINION_WALK_SPEED = 2.f;
static const float MINION_TARGET_TOUCH_RADIUS = .5f;
static const float MINION_TARGET_DETECT_RANGE = 4.f;
static const float MINION_ATTACK_RANGE = 3.f;
static const int MINION_ATTACK_SWITCH_IDLE_DELAY = 10;
static const int MINION_ATTACK_DELAY = 120;
static const uint32_t MINION_RADIUS_CHECK_INTERVALE = 15;

uint32_t Minion::s_radiusCheckId = 0;

Minion::Minion()
    : radiusCheckId(s_radiusCheckId++)
{
}

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
            // Check if there is an ennemy in range
            //if ((radiusCheckId + Globals::rts_frame) % MINION_RADIUS_CHECK_INTERVALE == 0)
            //{
            //    auto units = Globals::pMap->getUnits();
            //    for (auto pUnit : units)
            //    {
            //        if (pUnit == this) continue;
            //        if (pUnit->team != team && pUnit->team != -1)
            //        {
            //            if (Vector2::DistanceSquared(position, pUnit->getCenter()) <= MINION_ATTACK_RANGE * MINION_ATTACK_RANGE)
            //            {
            //                // À L'ATTAQUE!
            //                attack(pUnit);
            //            }
            //        }
            //    }
            //}
            //if (pWaypoint)
            //{
            //    targetPos = pWaypoint->getCenter();
            //}
            if (Vector2::DistanceSquared(position, targetPos) <= MINION_TARGET_TOUCH_RADIUS * MINION_TARGET_TOUCH_RADIUS)
            {
                // Randomly pick a free spot on the map
                while (path.empty())
                {
                    while (true)
                    {
                        auto randomPos = onut::rand2f(Vector2(1, 1), Vector2(126, 126));
                        if (!Globals::pMap->collisions[(int)randomPos.x + (int)randomPos.y * 128])
                        {
                            // Find the path to get there
                            float totalCost = 0;
                            int result = Globals::pMap->findPath(position, randomPos, &path, &totalCost);

                            break;
                        }
                    }
                }
                targetPos = path.front();
                path.erase(path.begin());

                //if (!pWaypoint)
                //{
                //    idle();
                //}
                //else
                //{
                //    auto pNextWaypoint = dynamic_cast<Waypoint*>(pWaypoint);
                //    if (pNextWaypoint)
                //    {
                //        walkTo(pNextWaypoint->pNext[team]);
                //    }
                //    else
                //    {
                //        pWaypoint = nullptr;
                //        idle();
                //    }
                //}
            }
            break;
        }
        case eState::ATTACK:
        {
            ++attackDelay;
            if (attackDelay == MINION_ATTACK_SWITCH_IDLE_DELAY)
            {
                anim.pAnimRes = &Globals::baltAnimsResources[direction | BALT_IDLE][team];
                restartAnim();
            }
            else if (attackDelay == MINION_ATTACK_DELAY)
            {
                attackDelay = 0;
                anim.pAnimRes = &Globals::baltAnimsResources[direction | BALT_ATTACK][team];
                restartAnim();
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

void Minion::attack(Unit *in_pTarget)
{
    if (state == eState::ATTACK && pTarget == in_pTarget) return;
    pTarget = in_pTarget;
    state = eState::ATTACK;
    attackDelay = 0;

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
    else if (state == eState::ATTACK)
    {
        if (attackDelay < MINION_ATTACK_SWITCH_IDLE_DELAY)
        {
            if (pTarget)
            {
                targetPos = pTarget->getCenter();
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
                    anim.pAnimRes = &Globals::baltAnimsResources[direction | BALT_ATTACK][team];
                }
            }
        }
    }
}

void Minion::restartAnim()
{
    anim.progress = 0.f;
    anim.frame = 0;
}

#if _DEBUG
void Minion::renderDebug()
{
    return;
    if (state == eState::WALK_TO)
    {
        OPB->begin(onut::ePrimitiveType::LINE_STRIP);
        OPB->draw(position, {1, 1, 0, 1});
        Vector2 posf;
        for (decltype(path.size()) i = 0; i < path.size(); ++i)
        {
            OPB->draw(path[i], {1, 1, 0, 1});
        }
        OPB->end();
    }
}
#endif
