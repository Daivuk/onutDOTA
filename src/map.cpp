#include "Globals.h"
#include "map.h"

Map::Map(int seed)
    : m_tiledMap("../../assets/maps/daivuk.tmx")
    , m_cameraPos(64, 64)
{
}

Map::~Map()
{
}

void Map::render()
{
    auto camX = round(m_cameraPos.x * 40 - OScreenWf * .5f);
    auto camY = round(m_cameraPos.y * 40 - OScreenHf * .5f);
    m_tiledMap.setTransform(Matrix::CreateTranslation(-camX, -camY, 0));
    m_tiledMap.render({0, 0, 128, 128});
}

void Map::update()
{
}
