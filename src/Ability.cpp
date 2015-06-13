#include "Ability.h"
#include "Unit.h"
#include "Globals.h"

Ability::Ability(Unit *in_pOwner)
    : pOwner(in_pOwner)
{
}

void Ability::trigger(const Vector2 &in_position)
{
    position = in_position;
    coolDown = getCoolDown();
}

void Ability::activate()
{
    switch (getType())
    {
        case eAbilityType::AREA:
            trigger(pOwner->position);
            break;
        case eAbilityType::LINE:
            isActive = true;
            break;
        case eAbilityType::TARGET:
            isActive = true;
            break;
        case eAbilityType::TARGET_AREA:
            isActive = true;
            break;
    }
}

void Ability::rts_update()
{
    if (coolDown > 0)
    {
        coolDown -= ODT;
    }
    if (isActive)
    {
        if (OInput->isStateJustDown(DIK_ESCAPE) ||
            OInput->isStateJustDown(DIK_MOUSEB2))
        {
            cancel();
        }
        else if (OInput->isStateJustDown(DIK_MOUSEB1))
        {
            auto mapPos = Globals::pMap->screenToMap(OMousePos);
            bool isInRange = Vector2::DistanceSquared(pOwner->position, mapPos) <= getRange() * getRange();
            if (isInRange)
            {
                cancel();
                trigger(mapPos);
            }
        }
    }
}

void Ability::render()
{
    if (isActive)
    {
        auto mapPos = Globals::pMap->screenToMap(OMousePos);
        switch (getType())
        {
            case eAbilityType::AREA:
                break;
            case eAbilityType::LINE:
                break;
            case eAbilityType::TARGET:
                break;
            case eAbilityType::TARGET_AREA:
            {
                OSB->drawSprite(OGetTexture("dashedrange.png"), pOwner->position, {1, 1, 1, .5f}, 0.f, getRange() / 246.f);
                bool isInRange = Vector2::DistanceSquared(pOwner->position, mapPos) <= getRange() * getRange();
                if (isInRange)
                {
                    OSB->drawSprite(OGetTexture("abilityTarget.png"), mapPos, {.5f, 1, .5f, 1}, 0.f, getRadius() / 62.f);
                }
                else
                {
                    OSB->drawSprite(OGetTexture("abilityTarget.png"), mapPos, {1, 0, 0, 1}, 0.f, getRadius() / 62.f);
                }
                break;
            }
        }
    }
}
