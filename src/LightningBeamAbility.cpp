#include "LightningBeamAbility.h"
#include "Globals.h"

LightningBeamAbility::LightningBeamAbility(Unit *in_pOwner)
    : Ability(in_pOwner)
{
}

void LightningBeamAbility::rts_update()
{
    Ability::rts_update();

    if (isInstance)
    {
    }
}

void LightningBeamAbility::render()
{
    Ability::render();
}

void LightningBeamAbility::trigger(const Vector2 &in_position)
{
    Ability::trigger(in_position);

    auto pInstance = new LightningBeamAbility(*this);
    Globals::pMap->spawnAbility(pInstance);
    pInstance->isInstance = true;
    pInstance->triggerOnField(in_position);
}

void LightningBeamAbility::triggerOnField(const Vector2 &in_position)
{
    position = in_position;
}
