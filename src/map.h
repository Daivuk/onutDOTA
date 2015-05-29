#pragma once
#include "TiledMap.h"
#include "Unit.h"

#define MAX_UNITS 2048

class Map : public onut::Object
{
public:
    Map(int seed);
    virtual ~Map();

    void render();
    void update();
    void rts_update();

    Unit *spawn(const Vector2 &position, eUnitType unitType, int team);

public:
    onut::TiledMap m_tiledMap;
    Vector2 m_cameraPos;
    onut::Pool<sizeof(Unit), MAX_UNITS, 4U, false> m_unitPool;
    Unit *pUnitStart = nullptr;
    uint8_t *pCollisions = nullptr;
};
