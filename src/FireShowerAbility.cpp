#include "FireShowerAbility.h"
#include "FallingFireBall.h"
#include "Globals.h"

#define FIRE_BALL_COUNT 10
#define FILE_BALL_INTERFACE .2f

FireShowerAbility::FireShowerAbility(Unit *in_pOwner)
    : Ability(in_pOwner)
{
}

void FireShowerAbility::rts_update()
{
    Ability::rts_update();

    if (isInstance)
    {
        fireBallSpawnTimer += ODT;
        retain();
        while (fireBallSpawnTimer >= FILE_BALL_INTERFACE)
        {
            fireBallSpawnTimer -= FILE_BALL_INTERFACE;

            if (pOwner)
            {
                float angle = onut::randf(0, DirectX::XM_2PI);
                float radius = std::sqrtf(onut::randf(0, getRadius()));
                float x = position.x + radius * std::cosf(angle);
                float y = position.y + radius * std::sinf(angle);
                Globals::pMap->spawn(Vector2{x, y} + FIRE_BALL_START_OFFSET, eUnitType::FALLING_FIRE_BALL, pOwner->team, true);
            }

            ++fireBallCount;
            if (fireBallCount >= FIRE_BALL_COUNT)
            {
                Globals::pMap->destroyAbility(this);
                break;
            }
        }
        release();
    }
}

void FireShowerAbility::render()
{
    Ability::render();
}

void FireShowerAbility::trigger(const Vector2 &in_position)
{
    Ability::trigger(in_position);

    auto pInstance = new FireShowerAbility(*this);
    Globals::pMap->spawnAbility(pInstance);
    pInstance->isInstance = true;
    pInstance->triggerOnField(in_position);
}

void FireShowerAbility::triggerOnField(const Vector2 &in_position)
{
    position = in_position;
}
