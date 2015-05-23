#pragma once
#include "onut.h"
#include "eg.h"
#include "TiledMap.h"

class Map : public onut::Object
{
public:
    Map(int seed);
    virtual ~Map();

    void render();
    void update();

private:
    onut::TiledMap m_tiledMap;
    Vector2 m_cameraPos;
};
