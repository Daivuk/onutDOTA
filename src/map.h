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
    Unit *getUnitByMapId(uint32_t mapId);
    template<typename Tunit>
    std::vector<Tunit*> getUnits()
    {
        std::vector<Tunit*> ret;
        for (auto pUnit : m_units)
        {
            auto pTunit = dynamic_cast<Tunit*>(pUnit);
            if (pTunit)
            {
                ret.push_back(pTunit);
            }
        }
        return std::move(ret);
    }

public:
    onut::TiledMap m_tiledMap;
    Vector2 m_cameraPos;
    OPool *pUnitPool = nullptr;
    std::list<Unit *> m_units;
    uint8_t *pCollisions = nullptr;
};
