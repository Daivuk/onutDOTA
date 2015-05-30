#pragma once
#include "TiledMap.h"
#include "Waypoint.h"

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
    Unit *getUnitByMapId(uint32_t mapId);

public:
    onut::TiledMap m_tiledMap;
    Vector2 m_cameraPos;
    onut::Pool<sizeof(Waypoint), MAX_UNITS, 4U, false> m_unitPool;
    std::list<Unit *> m_units;
    uint8_t *pCollisions = nullptr;
};
