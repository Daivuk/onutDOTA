#pragma once
#include "Ability.h"

class FireShowerAbility : public Ability
{
public:
    FireShowerAbility(Unit *in_pOwner);

    void rts_update() override;
    void render() override;

    eAbilityType getType() const override { return eAbilityType::TARGET_AREA; }
    eAbilityUsage getUsage() const override { return eAbilityUsage::ACTIVE; }
    float getRadius() const override { return 1.5f; }
    float getRange() const { return 10.f; }
};
