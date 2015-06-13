#pragma once
#include "Ability.h"

class FireShowerAbility : public Ability
{
public:
    FireShowerAbility(Unit *in_pOwner);

    const char* iconFilename() const { return "icons/ability_fallingFireBall.png"; };

    void rts_update() override;
    void render() override;
    void trigger(const Vector2 &position) override;
    void triggerOnField(const Vector2 &position) override;

    eAbilityType getType() const override { return eAbilityType::TARGET_AREA; }
    eAbilityUsage getUsage() const override { return eAbilityUsage::ACTIVE; }
    float getRadius() const override { return 1.5f; }
    float getRange() const { return 10.f; }
    float getCoolDown() const { return 5.f; }

    float fireBallSpawnTimer = 0.f;
    int fireBallCount = 0;
};
