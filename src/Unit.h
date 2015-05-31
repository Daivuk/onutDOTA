#pragma once

#include "onut.h"

#define TEAM_NONE -1
#define TEAM_RED 0
#define TEAM_BLUE 1

enum class eUnitType : uint8_t
{
    NONE,
    SPAWNER,
    NEXUS,
    WAYPOINT,
    MINION
};

enum class eUnitSizeType : uint8_t
{
    NONE,
    RADIUS,
    BOX
};

class Unit
{
public:
    virtual void render();
    virtual void rts_update();
    Vector2 getCenter() const;
#if _DEBUG
    virtual void renderDebug() {}
#endif

    eUnitType type = eUnitType::NONE;
    eUnitSizeType sizeType = eUnitSizeType::RADIUS;
    Vector2 position;
    POINT boxSize;
    OTexture *pTexture = nullptr;
    Vector4 spriteOffsetAndSize;
    Vector4 UVs = {0, 0, 1, 1};
    float radius = 0.4f;
    int team = TEAM_NONE;
    uint32_t mapId = 0;
    int health = 100;
    int armor = 0;
    int mana = 0;
    Unit *pTarget = nullptr;
};
