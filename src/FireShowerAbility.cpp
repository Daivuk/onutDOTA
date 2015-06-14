#include "FireShowerAbility.h"
#include "FallingFireBall.h"
#include "Globals.h"

#define FIRE_BALL_COUNT 10
#define FILE_BALL_INTERFACE .2f

FireShowerAbility::FireShowerAbility()
{
}

void FireShowerAbility::rts_update()
{
    Ability::rts_update();

    if (isInstance)
    {
        fireBallSpawnTimer += ODT;
        while (fireBallSpawnTimer >= FILE_BALL_INTERFACE)
        {
            fireBallSpawnTimer -= FILE_BALL_INTERFACE;

            if (pOwner)
            {
                float angle = onut::randf(0, DirectX::XM_2PI);
                float radius = std::sqrtf(onut::randf(0, getAbilityRadius()));
                float x = position.x + radius * std::cosf(angle);
                float y = position.y + radius * std::sinf(angle);
                Globals::pMap->spawn(Vector2{x, y} + FIRE_BALL_START_OFFSET, eUnitType::FALLING_FIRE_BALL, pOwner->team, true);
            }

            ++fireBallCount;
            if (fireBallCount >= FIRE_BALL_COUNT)
            {
                markForDeletion();
                break;
            }
        }
    }
}

void FireShowerAbility::render()
{
    Ability::render();
}

void FireShowerAbility::triggerAbility(const Vector2 &in_position)
{
    Ability::triggerAbility(in_position);

    if (pOwner)
    {
        auto pAbility = dynamic_cast<FireShowerAbility*>(Globals::pMap->spawn(in_position, eUnitType::ABILITY_FIRE_SHOWER, pOwner->team));
        pAbility->isInstance = true;
        pAbility->pOwner = pOwner;
        pAbility->onSpawn();
    }
}
