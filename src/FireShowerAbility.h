#pragma once
#include "Ability.h"

class FireShowerAbility : public Ability
{
public:
    FireShowerAbility();

    const char* iconFilename() const { return "icons/ability_fallingFireBall.png"; };

    void rts_update() override;
    void render() override;
    void triggerAbility(const Vector2 &position) override;

    eAbilityType getAbilityType() const override { return eAbilityType::TARGET_AREA; }
    eAbilityUsage getAbilityUsage() const override { return eAbilityUsage::ACTIVE; }
    float getAbilityRadius() const override { return 1.5f; }
    float getAbilityRange() const override { return 10.f; }
    float getAbilityCoolDown() const override { return 5.f; }

    float fireBallSpawnTimer = 0.f;
    int fireBallCount = 0;
};
