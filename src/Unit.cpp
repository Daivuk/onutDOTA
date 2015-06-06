#include "Unit.h"
#include "map.h"
#include "Globals.h"

static const float UNIT_TARGET_TOUCH_RADIUS = 1.5f;
static const float UNIT_STEER_RANGE = 0.75f;

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
}

void Unit::rts_update()
{
    if (type == eUnitType::NONE) return;

    if (pType->category != eUnitCategory::NONE)
    {
        movingDirection = {0, 0};

        // Do movement
        rts_updateState();

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
    switch (state)
    {
        case eUnitState::ATTACK_POSITION:
        {
            doMovement();
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
    state = eUnitState::IDLE;
}

void Unit::attackTo(const Vector2 &attackPos)
{
    state = eUnitState::ATTACK_POSITION;
    calculatePathToPos(attackPos);
}

void Unit::calculatePathToPos(const Vector2 &in_targetPos)
{
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
        onReachDestination();
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
}
#endif
