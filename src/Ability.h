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

class Ability : public onut::Object
{
public:
    Ability(Unit *in_pOwner);

    virtual void trigger(const Vector2 &in_position);
    virtual void triggerOnField(const Vector2 &in_position) {}
    virtual void activate();
    void cancel() { isActive = false; }

    virtual void rts_update();
    virtual void render();

    bool canUse() const { return coolDown <= 0.f; }

    // Type info
    virtual eAbilityType    getType() const = 0;
    virtual eAbilityUsage   getUsage() const = 0;
    virtual float           getRadius() const { return 0.f; }
    virtual float           getRange() const { return 0.f; }
    virtual float           getCoolDown() const { return 0.f; }

    Unit *pOwner = nullptr;
    bool isActive = false;
    Vector2 position;
    bool isInstance = false;
    float coolDown = 0;
};
