#pragma once
#include "TiledMap.h"
#include "Unit.h"
#include "micropather.h"

#define MAX_UNITS 2048
#define CHUNK_SIZE 4

struct sMapChunk
{
    TList<Unit> *pUnits;

    sMapChunk();
    ~sMapChunk();
};

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
        for (auto pUnit = pUnits->Head(); pUnit; pUnit = pUnits->Next(pUnit))
        {
            auto pTunit = dynamic_cast<Tunit*>(pUnit);
            if (pTunit)
            {
                ret.push_back(pTunit);
            }
        }
        return std::move(ret);
    }
    TList<Unit> *getUnits() const { return pUnits; }

    sMapChunk *getChunkAt(const Vector2 &pos)
    {
        if (pos.x < 0 || pos.y < 0 || pos.x >= (float)m_tiledMap.getWidth() || pos.y >= (float)m_tiledMap.getHeight()) return nullptr;
        int chunkX = (int)pos.x / CHUNK_SIZE;
        int chunkY = (int)pos.y / CHUNK_SIZE;
        auto chunkIdk = chunkY * chunkYCount + chunkX;
        return pChunks + chunkIdk;
    }

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
    sMapChunk *pChunks;
    Vector2 m_cameraPos;
    OPool *pUnitPool = nullptr;
    TList<Unit> *pUnits = nullptr;
    bool *collisions = nullptr;
    micropather::MicroPather *pPather = nullptr;
    int entityLayerIndex = 0;
    int chunkXCount = 0;
    int chunkYCount = 0;
};
