#pragma once
#include "onut.h"
#include "Unit.h"

enum class eAbilityType : uint8_t
{
    AREA,
    TARGET_AREA,
    LINE,
    TARGET
};

enum class eAbilityUsage : uint8_t
{
    PASSIVE,
    ACTIVE
};

class Ability : public Unit
{
public:
    Ability();

    virtual const char* iconFilename() const = 0;
    virtual void triggerAbility(const Vector2 &in_position);
    virtual void triggerAbility(Unit *in_pTarget);
    virtual void activateAbility();
    void cancelAbility() { isAbilityActive = false; }

    virtual void rts_update() override;
    virtual void render() override;

    bool canUseAbility() const { return abilityCoolDown <= 0.f; }

    // Type info
    virtual eAbilityType    getAbilityType() const = 0;
    virtual eAbilityUsage   getAbilityUsage() const = 0;
    virtual float           getAbilityRadius() const { return 0.f; }
    virtual float           getAbilityRange() const { return 0.f; }
    virtual float           getAbilityCoolDown() const { return 0.f; }

    bool isAbilityActive = false;
    bool isInstance = false;
    float abilityCoolDown = 0;
};
