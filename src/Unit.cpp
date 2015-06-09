#include "Unit.h"
#include "map.h"
#include "Globals.h"

static const float UNIT_TARGET_TOUCH_RADIUS = 1.5f;
static const float UNIT_STEER_RANGE = 0.75f;
static const float UNIT_TARGET_ESCAPE_RANGE = 15.f;
static const float UNIT_TARGET_ESCAPE_RANGE_SQR = UNIT_TARGET_ESCAPE_RANGE * UNIT_TARGET_ESCAPE_RANGE;

Unit::Unit()
    : pChunk(nullptr)
{
}

void Unit::render()
{
    if (type == eUnitType::NONE) return;
    if (pType->category == eUnitCategory::NONE) return;

    auto &frame = anim.pAnimDef->frames[anim.frame];
    auto UV = frame.UVs;
    if (team == 1)
    {
        UV.y += .5f;
        UV.w += .5f;
    }
    OSB->drawRectWithUVs(anim.pAnimDef->pTexture,
                         {position.x + frame.offset.x, position.y + frame.offset.y, frame.size.x, frame.size.y},
                         UV);

    if (health < pType->health)
    {
        OSB->drawRect(nullptr,
            {position.x + frame.offset.x - 1.f / 40.f, position.y + frame.offset.y - 4.f / 40.f, frame.size.x + 2.f / 40.f, 4.f / 40.f},
            {0, 0, 0, 1});
        float percent = ((float)health / (float)pType->health);
        static const Color colorFULL{0, 1, 0, 1};
        static const Color colorMid{1, 1, 0, 1};
        static const Color colorEmpty{1, 0, 0, 1};
        Color color = colorFULL;
        if (percent > .5f)
        {
            color = Color::Lerp(colorMid, colorFULL, percent * 2.f - 1.f);
        }
        else 
        {
            color = Color::Lerp(colorEmpty, colorMid, percent * 2.f);
        }
        OSB->drawRect(nullptr,
            {position.x + frame.offset.x, position.y + frame.offset.y - 3.f / 40.f, frame.size.x * percent, 2.f / 40.f},
            color);
    }
}

void Unit::rts_update()
{
    if (type == eUnitType::NONE) return;

    if (pType->category != eUnitCategory::NONE)
    {
        movingDirection = {0, 0};

        // Attack cooldown
        if (attackCooldown > 0) attackCooldown -= ODT;
        if (attackDelay > 0)
        {
            attackDelay -= ODT;
            if (attackDelay <= 0)
            {
                performAttack();
            }
        }

        // Update animation
        anim.progress += ODT * (float)anim.pAnimDef->fps;
        anim.frame = (int)anim.progress;
        if (anim.frame >= anim.pAnimDef->frameCount)
        {
            if (animState == BALT_ATTACK)
            {
                animState = BALT_IDLE;
                anim.frame = 0;
                anim.progress = 0.f;
                animDirty = true;
            }
            else
            {
                anim.frame = (int)anim.progress % anim.pAnimDef->frameCount;
            }
        }

        // State update
        rts_updateState();

        // Update animations based on direction and movement
        if (animState == BALT_IDLE)
        {
            if (movingDirection.LengthSquared() > 0)
            {
                animState = BALT_WALK;
                anim.frame = 0;
                anim.progress = 0.f;
                animDirty = true;
                movingDirection = {0, 0};
            }
        }
        else if (animState == BALT_WALK)
        {
            if (movingDirection.LengthSquared() == 0)
            {
                animState = BALT_IDLE;
                anim.frame = 0;
                anim.progress = 0.f;
                animDirty = true;
            }
        }

        if (animDirty)
        {
            anim.pAnimDef = pType->anims[animState | direction];
            animDirty = false;
        }
    }
}

void Unit::rts_updateState()
{
    if (pType->category == eUnitCategory::BUILDLING)
    {
        if (animState != BALT_ATTACK && pType->attackType != eUnitAttackType::NONE)
        {
            if (pTarget)
            {
                if (attackCooldown <= 0)
                {
                    if (targetInAttackRange())
                    {
                        animState = BALT_ATTACK;
                        anim.frame = 0;
                        anim.progress = 0.f;
                        animDirty = true;
                        attackDelay = pType->attackDelay;
                        attackCooldown = pType->attackCoolDown;
                    }
                    else
                    {
                        pTarget = nullptr;
                    }
                }
            }
            else
            {
                pTarget = aquireTarget();
            }
        }
        return;
    }

    switch (state)
    {
        case eUnitState::ATTACK_POSITION:
        {
            if (animState != BALT_ATTACK)
            {
                // If we are not currently attacking, update target state
                if (pType->attackType != eUnitAttackType::NONE)
                {
                    if (pTarget)
                    {
                        if (targetEscaped())
                        {
                            pTarget = nullptr;
                            targetPos = prevTargetPos;
                            calculatePathToPos(targetPos);
                        }
                        else if (attackCooldown <= 0)
                        {
                            if (targetInAttackRange())
                            {
                                animState = BALT_ATTACK;
                                anim.frame = 0;
                                anim.progress = 0.f;
                                animDirty = true;
                                attackDelay = pType->attackDelay;
                                attackCooldown = pType->attackCoolDown;
                            }
                            else
                            {
                                // Recalculate path when he goes out of our targetpos attack range 
                                if (Vector2::DistanceSquared(pTarget->position, missionTargetPos) >= pType->attackRange * .5f)
                                {
                                    calculatePathToPos(pTarget->position);
                                }
                                doMovement();
                            }
                        }
                    }
                    else
                    {
                        doMovement();
                        if (!pTarget)
                        {
                            pTarget = aquireTarget();
                            if (pTarget)
                            {
                                prevTargetPos = missionTargetPos;
                            }
                        }
                    }
                }
                else
                {
                    doMovement();
                }
            }
            break;
        }
        case eUnitState::MOVE:
        {
            if (animState != BALT_ATTACK)
            {
                doMovement();
            }
            break;
        }
        case eUnitState::IDLE:
        {
            // If we are not currently attacking, update target state
            if (pType->attackType != eUnitAttackType::NONE)
            {
                if (!pTarget)
                {
                    pTarget = aquireTarget();
                    if (pTarget)
                    {
                        attackTo(getCenter());
                    }
                }
            }
            break;
        }
        default: break;
    }
}

Vector2 Unit::getCenter() const
{
    if (pType->sizeType == eUnitSizeType::BOX)
    {
        return position + Vector2((float)boxSize.x * .5f, (float)boxSize.y * .5f);
    }
    return position;
}

void Unit::goIdle()
{
    pTarget = nullptr;
    state = eUnitState::IDLE;
}

void Unit::attackTo(const Vector2 &attackPos)
{
    state = eUnitState::ATTACK_POSITION;
    calculatePathToPos(attackPos);
}

void Unit::moveTo(const Vector2 &movePos)
{
    state = eUnitState::MOVE;
    calculatePathToPos(movePos);
}

void Unit::calculatePathToPos(const Vector2 &in_targetPos)
{
    missionTargetPos = in_targetPos;
    float totalCost = 0;
    int result = Globals::pMap->findPath(position, in_targetPos, &path, &totalCost);
    path.push_back(in_targetPos);
    targetPos = path.front();
    path.erase(path.begin());
}

void Unit::progressPath()
{
    if (path.empty())
    {
        goIdle();
        if (!pTarget)
        {
            onReachDestination();
        }
        return;
    }
    targetPos = path.front();
    path.erase(path.begin());
}

void Unit::steer(const Vector2 &otherPos, Vector2 &dir, float strength)
{
    Vector2 dirWithOther = otherPos - position;
    dirWithOther.Normalize();
    Vector2 steerDir = dir - dirWithOther;
    if (steerDir.LengthSquared() == 0)
    {
        steerDir.x = -dir.y;
        steerDir.y = dir.x;
    }
    steerDir.Normalize();
    dir += steerDir * strength;
    dir.Normalize();
}

void Unit::steerFromTile(int tileX, int tileY, Vector2 &dir)
{
    if (Globals::pMap->passable(tileX, tileY)) return;

    Vector2 tilePos{(float)tileX + .5f, (float)tileY + .5f};
    float dis = Vector2::DistanceSquared(position, tilePos);
    if (dis <= UNIT_STEER_RANGE * UNIT_STEER_RANGE)
    {
        dis = sqrtf(dis);
        float steerForce = (dis - pType->radius) / (UNIT_STEER_RANGE - pType->radius);
        steerForce = 1 - pow(steerForce, 3.f);
        steer(tilePos, dir, steerForce);
    }
}

void Unit::doMovement()
{
    // Calculate direction vector
    if (pType->category == eUnitCategory::GROUND)
    {
        movingDirection = targetPos - position;
        movingDirection.Normalize();

        float radius = pType->radius;

        // Steer against others
        int chunkFromX = (int)(position.x - UNIT_STEER_RANGE - radius) / CHUNK_SIZE;
        int chunkFromY = (int)(position.y - UNIT_STEER_RANGE - radius) / CHUNK_SIZE;
        int chunkToX = (int)(position.x + UNIT_STEER_RANGE + radius) / CHUNK_SIZE;
        int chunkToY = (int)(position.y + UNIT_STEER_RANGE + radius) / CHUNK_SIZE;

        for (int chunkY = chunkFromY; chunkY <= chunkToY; ++chunkY)
        {
            for (int chunkX = chunkFromX; chunkX <= chunkToX; ++chunkX)
            {
                auto pChunk = Globals::pMap->pChunks + (chunkY * Globals::pMap->chunkXCount + chunkX);
                for (auto pUnit = pChunk->pUnits->Head(); pUnit; pUnit = pChunk->pUnits->Next(pUnit))
                {
                    if (pUnit == this) continue;
                    if (pUnit->pType->category != eUnitCategory::GROUND) continue;
                    float dis = Vector2::DistanceSquared(position, pUnit->position);
                    if (dis <= UNIT_STEER_RANGE * UNIT_STEER_RANGE + radius * radius)
                    {
                        dis = sqrtf(dis);
                        dis -= radius;
                        float steerForce = 1.f - (dis - radius) / (UNIT_STEER_RANGE - radius);
                        steer(pUnit->position, movingDirection, steerForce);
                    }
                }
            }
        }

        position += movingDirection * pType->moveSpeed * ODT;

        // Update direction based on the dir
        auto oldDirection = direction;
        if (movingDirection.y > 0.f && std::abs(movingDirection.x) <= std::abs(movingDirection.y))
        {
            direction = BALT_DOWN;
        }
        else if (movingDirection.y < 0.f && std::abs(movingDirection.x) <= std::abs(movingDirection.y))
        {
            direction = BALT_UP;
        }
        else if (movingDirection.x > 0.f)
        {
            direction = BALT_RIGHT;
        }
        else
        {
            direction = BALT_LEFT;
        }
        if (direction != oldDirection) animDirty = true;
    }

    if (Vector2::DistanceSquared(position, targetPos) <= UNIT_TARGET_TOUCH_RADIUS * UNIT_TARGET_TOUCH_RADIUS)
    {
        progressPath();
    }
}

bool Unit::targetEscaped()
{
    if (!pTarget) return true;
    if (Vector2::DistanceSquared(position, pTarget->position) >= UNIT_TARGET_ESCAPE_RANGE_SQR)
    {
        return true;
    }
    return false;
}

bool Unit::targetInAttackRange()
{
    if (!pTarget) return false;
    if (Vector2::DistanceSquared(position, pTarget->position) <= pType->attackRange * pType->attackRange)
    {
        return true;
    }
    return false;
}

Unit* Unit::aquireTarget()
{
    float radius = pType->radius;

    // Steer against others
    int chunkFromX = (int)(position.x - pType->alertRange) / CHUNK_SIZE;
    int chunkFromY = (int)(position.y - pType->alertRange) / CHUNK_SIZE;
    int chunkToX = (int)(position.x + pType->alertRange) / CHUNK_SIZE;
    int chunkToY = (int)(position.y + pType->alertRange) / CHUNK_SIZE;

    Unit *pClosest = nullptr;
    float closestDis = pType->alertRange * pType->alertRange;

    for (int chunkY = chunkFromY; chunkY <= chunkToY; ++chunkY)
    {
        for (int chunkX = chunkFromX; chunkX <= chunkToX; ++chunkX)
        {
            auto pChunk = Globals::pMap->pChunks + (chunkY * Globals::pMap->chunkXCount + chunkX);
            for (auto pUnit = pChunk->pUnits->Head(); pUnit; pUnit = pChunk->pUnits->Next(pUnit))
            {
                if (pUnit == this) continue;
                if (pUnit->team == team || pUnit->team == TEAM_NONE) continue;
                if (pUnit->pType->category != eUnitCategory::AIR &&
                    pUnit->pType->category != eUnitCategory::GROUND &&
                    pUnit->pType->category != eUnitCategory::BUILDLING) continue;
                float dis = Vector2::DistanceSquared(position, pUnit->position);
                if (dis < closestDis)
                {
                    closestDis = dis;
                    pClosest = pUnit;
                }
            }
        }
    }

    return pClosest;
}

void Unit::performAttack()
{
    if (!pTarget) return;
    if (pType->attackType == eUnitAttackType::PROJECTILE)
    {
        auto pProjectile = Globals::pMap->spawn(position, pType->projectileUnitType, team);
        if (pProjectile)
        {
            pProjectile->pTarget = pTarget;
            pProjectile->pOwner = this;
            pProjectile->onSpawn();
        }
    }
}

void Unit::markForDeletion()
{
    bDeletionRequested = true;
}

bool Unit::damage(float damage)
{
    float finalDamage = onut::randf(damage - damage * .25f, damage + damage * .25f);
    int dmgi = (int)roundf(finalDamage);
    dmgi -= pType->armor;
    dmgi = std::max<>(1, dmgi);
    health = std::max<>(0, health - dmgi);
    if (health == 0)
    {
        markForDeletion();
        return true;
    }
    return false;
}

#if _DEBUG
void Unit::renderDebug()
{
    OPB->begin(onut::ePrimitiveType::LINE_STRIP);
    OPB->draw(position, {1, 1, 0, 1});
    Vector2 posf;
    for (decltype(path.size()) i = 0; i < path.size(); ++i)
    {
        OPB->draw(path[i], {1, 1, 0, 1});
    }
    OPB->end();

    if (pTarget)
    {
        OPB->begin(onut::ePrimitiveType::LINES);
        OPB->draw(position, {1, 0, 0, 1});
        OPB->draw(pTarget->position, {1, 0, 0, 1});
        OPB->end();
    }

    if (pType->category == eUnitCategory::BUILDLING)
    {
        OPB->begin(onut::ePrimitiveType::LINE_STRIP);
        OPB->draw(position, {0, 1, 1, 1});
        OPB->draw(position + Vector2(0, (float)boxSize.y), {0, 1, 1, 1});
        OPB->draw(position + Vector2((float)boxSize.x, (float)boxSize.y), {0, 1, 1, 1});
        OPB->draw(position + Vector2((float)boxSize.x, 0), {0, 1, 1, 1});
        OPB->draw(position, {0, 1, 1, 1});
        OPB->end();
    }
}
#endif
