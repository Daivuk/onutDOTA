#pragma once
#include "Ability.h"

class LightningBeamAbility : public Ability
{
public:
    LightningBeamAbility();

    const char* iconFilename() const { return "icons/ability_lightningBeam.png"; };

    void rts_update() override;
    void render() override;
    void triggerAbility(Unit *in_pTarget) override;
    void onSpawn() override;

    eAbilityType getAbilityType() const override { return eAbilityType::TARGET; }
    eAbilityUsage getAbilityUsage() const override { return eAbilityUsage::ACTIVE; }
    float getAbilityRange() const override { return 8.f; }
    float getAbilityCoolDown() const override { return 5.f; }

    float beamDuration = 2.5f;
    float beamAnim = 0.f;
    float beamDamageDelay = 0.f;
};
