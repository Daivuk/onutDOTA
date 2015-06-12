#include "FireShowerAbility.h"

FireShowerAbility::FireShowerAbility(Unit *in_pOwner)
    : Ability(in_pOwner)
{
}

void FireShowerAbility::rts_update()
{
    Ability::rts_update();
}

void FireShowerAbility::render()
{
    Ability::render();
}
