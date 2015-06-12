#pragma once
#include <cinttypes>
#include "onut.h"

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

class Unit;

class Ability
{
public:
    Ability(Unit *in_pOwner);

    void trigger(const Vector2 &position);
    virtual void activate();
    void cancel() { isActive = false; }

    virtual void rts_update();
    virtual void render();

    // Type info
    virtual eAbilityType    getType() const = 0;
    virtual eAbilityUsage   getUsage() const = 0;
    virtual float           getRadius() const { return 0.f; }
    virtual float           getRange() const { return 0.f; }

    Unit *pOwner = nullptr;
    bool isActive = false;
};
