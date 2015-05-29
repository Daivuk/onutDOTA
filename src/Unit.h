#pragma once

#include "onut.h"

enum class eUnitType : uint8_t
{
    NONE,
    SPAWNER
};

enum class eUnitSizeType : uint8_t
{
    RADIUS,
    BOX
};

class Unit
{
public:
    void render();
    void rts_update();

    Unit *pPrevious = nullptr;
    Unit *pNext = nullptr;

    eUnitType type = eUnitType::NONE;
    eUnitSizeType sizeType = eUnitSizeType::RADIUS;
    Vector2 position;
    POINT boxSize;
    OTexture *pTexture = nullptr;
    Vector4 spriteOffsetAndSize;
    Vector4 UVs = {0, 0, 1, 1};
    float radius = 0.4f;
    int team = -1;
};
