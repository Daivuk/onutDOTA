#include "Ability.h"
#include "Unit.h"
#include "Globals.h"

Ability::Ability()
{
}

void Ability::triggerAbility(const Vector2 &in_position)
{
    position = in_position;
    abilityCoolDown = getAbilityCoolDown();
}

void Ability::triggerAbility(Unit *in_pTarget)
{
    abilityCoolDown = getAbilityCoolDown();
}

void Ability::activateAbility()
{
    switch (getAbilityType())
    {
        case eAbilityType::AREA:
            triggerAbility(pOwner->position);
            break;
        case eAbilityType::LINE:
            isAbilityActive = true;
            break;
        case eAbilityType::TARGET:
            isAbilityActive = true;
            break;
        case eAbilityType::TARGET_AREA:
            isAbilityActive = true;
            break;
    }
}

void Ability::rts_update()
{
    if (abilityCoolDown > 0)
    {
        abilityCoolDown -= ODT;
    }
    if (isAbilityActive)
    {
        if (OInput->isStateJustDown(DIK_ESCAPE) ||
            OInput->isStateJustDown(DIK_MOUSEB2))
        {
            cancelAbility();
        }
        else if (OInput->isStateJustDown(DIK_MOUSEB1))
        {
            auto mapPos = Globals::pMap->screenToMap(OMousePos);
            bool isInRange = Vector2::DistanceSquared(pOwner->position, mapPos) <= getAbilityRange() * getAbilityRange();
            if (isInRange)
            {
                cancelAbility();
                switch (getAbilityType())
                {
                    case eAbilityType::TARGET:
                    {
                        if (pOwner)
                        {
                            auto pTarget = Globals::pMap->getUnitAt(mapPos);
                            if (pTarget)
                            {
                                triggerAbility(pTarget);
                            }
                        }
                        break;
                    }
                case eAbilityType::TARGET_AREA:
                    triggerAbility(mapPos);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void Ability::render()
{
    if (isAbilityActive)
    {
        auto mapPos = Globals::pMap->screenToMap(OMousePos);
        switch (getAbilityType())
        {
            case eAbilityType::AREA:
                break;
            case eAbilityType::LINE:
                break;
            case eAbilityType::TARGET:
            {
                auto pTarget = Globals::pMap->getUnitAt(mapPos);
                OSB->drawSprite(OGetTexture("dashedrange.png"), pOwner->position, {1, 1, 1, .5f}, 0.f, getAbilityRange() / 246.f);
                if (pTarget)
                {
                    OSB->drawSprite(OGetTexture("target.png"), mapPos, {1, 0, 0, 1}, 0.f, 1.5f / 40.f);
                }
                else
                {
                    OSB->drawSprite(OGetTexture("target.png"), mapPos, {.5f, 1, .5f, 1}, 0.f, 1.5f / 40.f);
                }
                break;
            }
            case eAbilityType::TARGET_AREA:
            {
                OSB->drawSprite(OGetTexture("dashedrange.png"), pOwner->position, {1, 1, 1, .5f}, 0.f, getAbilityRange() / 246.f);
                bool isInRange = Vector2::DistanceSquared(pOwner->position, mapPos) <= getAbilityRange() * getAbilityRange();
                if (isInRange)
                {
                    OSB->drawSprite(OGetTexture("abilityTarget.png"), mapPos, {.5f, 1, .5f, 1}, 0.f, getAbilityRadius() / 62.f);
                }
                else
                {
                    OSB->drawSprite(OGetTexture("abilityTarget.png"), mapPos, {1, 0, 0, 1}, 0.f, getAbilityRadius() / 62.f);
                }
                break;
            }
        }
    }
}
