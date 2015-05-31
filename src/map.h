#pragma once
#include "TiledMap.h"
#include "Unit.h"
#include "micropather.h"

#define MAX_UNITS 2048

class Map : public onut::Object, public micropather::Graph
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
    const std::list<Unit *> &getUnits() const { return m_units; }

    // A* stuff
    float LeastCostEstimate(void* stateStart, void* stateEnd) override;
    void AdjacentCost(void* state, MP_VECTOR< micropather::StateCost > *adjacent) override;
    void PrintStateInfo(void* state) override;
    int findPath(const Vector2 &from, const Vector2 &to, std::vector<Vector2> *path, float* cost);
    void nodeToXY(void* node, int* x, int* y);
    void* xyToNode(int x, int y);
    bool passable(int x, int y);

public:
    onut::TiledMap m_tiledMap;
    Vector2 m_cameraPos;
    OPool *pUnitPool = nullptr;
    std::list<Unit *> m_units;
    bool *collisions = nullptr;
    micropather::MicroPather *pPather = nullptr;
    int entityLayerIndex = 0;
};
