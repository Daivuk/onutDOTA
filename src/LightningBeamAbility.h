#pragma once
#include "Ability.h"

class LightningBeamAbility : public Ability
{
public:
    LightningBeamAbility(Unit *in_pOwner);

    const char* iconFilename() const { return "icons/ability_lightningBeam.png"; };

    void rts_update() override;
    void render() override;
    void trigger(const Vector2 &position) override;
    void triggerOnField(const Vector2 &position) override;

    eAbilityType getType() const override { return eAbilityType::TARGET; }
    eAbilityUsage getUsage() const override { return eAbilityUsage::ACTIVE; }
    float getRange() const { return 8.f; }
    float getCoolDown() const { return 5.f; }
};
