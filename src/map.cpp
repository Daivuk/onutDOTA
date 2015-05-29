#include "Globals.h"
#include "map.h"
#include "Unit.h"
#include "eg.h"

Map::Map(int seed)
    : m_tiledMap("../../assets/maps/daivuk.tmx")
    , m_cameraPos(64, 64)
{
    pCollisions = new uint8_t[m_tiledMap.getWidth() * m_tiledMap.getHeight()];

    // Read and spawn entities already on the map
    for (auto i = 0; i < m_tiledMap.getLayerCount(); ++i)
    {
        auto pObjectLayer = dynamic_cast<onut::TiledMap::sObjectLayer*>(m_tiledMap.getLayer(i));
        if (!pObjectLayer) continue;
        for (uint32_t j = 0; j < pObjectLayer->objectCount; ++j)
        {
            auto pObject = pObjectLayer->pObjects[j];
            int team;
            pObject.position /= 40.f;
            pObject.size /= 40.f;
            try
            {
                team = std::stoi(pObject.properties["team"]);
            }
            catch (std::exception e) 
            {
                continue;
            }
            if (pObject.type == "Spawner")
            {
                auto pUnit = spawn(pObject.position, eUnitType::SPAWNER, team);
            }
        }
    }
}

Map::~Map()
{
    delete[] pCollisions;
}

void Map::render()
{
    // Define our camera transform matrix
    auto camX = round(m_cameraPos.x * 40.f - OScreenWf * .5f);
    auto camY = round(m_cameraPos.y * 40.f - OScreenHf * .5f);
    Matrix transform = Matrix::CreateTranslation(-camX, -camY, 0);
    m_tiledMap.setTransform(transform);

    // Define the visible rectangle
    RECT rect;
    rect.left = static_cast<LONG>((m_cameraPos.x * 40 - OScreenWf * .5f) / 40.f);
    rect.top = static_cast<LONG>((m_cameraPos.y * 40 - OScreenHf * .5f) / 40.f);
    rect.right = static_cast<LONG>((m_cameraPos.x * 40 + OScreenWf * .5f) / 40.f);
    rect.bottom = static_cast<LONG>((m_cameraPos.y * 40 + OScreenHf * .5f) / 40.f);

    // Draw the visible part of the map
    m_tiledMap.render(rect);

    // Draw units
    transform = Matrix::CreateScale(40.f, 40.f, 1.f) * transform;
    egModelPush();
    egModelIdentity();
    egModelMult(&transform._11);

    OSB->begin();
    for (auto pUnit = pUnitStart; pUnit; pUnit = pUnit->pNext)
    {
        pUnit->render();
    }
    OSB->end();

    egModelPop();
}

void Map::update()
{
    // Get settings
    float keyScrollSpeed = std::stof(OSettings->getUserSetting("key_scroll_speed"));
    float mouseScrollSpeed = std::stof(OSettings->getUserSetting("mouse_scroll_speed"));
    float mouseScrollEdgeSize = std::stof(OSettings->getUserSetting("mouse_scroll_edge_size"));
    float dragScrollSpeed = std::stof(OSettings->getUserSetting("drag_scroll_speed"));

    // Do scrolling
    if ((OMousePos.x <= mouseScrollEdgeSize && OMousePos.x >= 0) || OInput->isStateDown(DIK_LEFT))
    {
        m_cameraPos.x -= mouseScrollSpeed * ODT;
    }
    if ((OMousePos.x >= OScreenWf - mouseScrollEdgeSize && OMousePos.x <= OScreenWf) || OInput->isStateDown(DIK_RIGHT))
    {
        m_cameraPos.x += mouseScrollSpeed * ODT;
    }
    if ((OMousePos.y <= mouseScrollEdgeSize && OMousePos.y >= 0) || OInput->isStateDown(DIK_UP))
    {
        m_cameraPos.y -= mouseScrollSpeed * ODT;
    }
    if ((OMousePos.y >= OScreenHf - mouseScrollEdgeSize && OMousePos.y <= OScreenHf) || OInput->isStateDown(DIK_DOWN))
    {
        m_cameraPos.y += mouseScrollSpeed * ODT;
    }

    // Clamp to map edges
    auto camX = m_cameraPos.x * 40 - OScreenWf * .5f;
    auto camY = m_cameraPos.y * 40 - OScreenHf * .5f;
    if (camX < 0) m_cameraPos.x = OScreenWf / 80.f;
    if (camY < 0) m_cameraPos.y = OScreenHf / 80.f;
    if (camX > static_cast<float>(m_tiledMap.getWidth()) * 40.f - OScreenWf)
    {
        m_cameraPos.x = static_cast<float>(m_tiledMap.getWidth()) - OScreenWf / 80.f;
    }
    if (camY > static_cast<float>(m_tiledMap.getHeight()) * 40.f - OScreenHf)
    {
        m_cameraPos.y = static_cast<float>(m_tiledMap.getHeight()) - OScreenHf / 80.f;
    }
}

void Map::rts_update()
{
    // Update units
    for (auto pUnit = pUnitStart; pUnit; pUnit = pUnit->pNext)
    {
        pUnit->rts_update();
    }
}

Unit *Map::spawn(const Vector2 &position, eUnitType unitType, int team)
{
    // Alloc a new unit
    auto pUnit = m_unitPool.alloc<Unit>();
    if (!pUnit) return nullptr;

    pUnit->position = position;
    pUnit->type = unitType;
    pUnit->team = team;

    switch (unitType)
    {
        case eUnitType::SPAWNER:
            pUnit->sizeType = eUnitSizeType::BOX;
            pUnit->boxSize = {3, 3};
            pUnit->pTexture = OGetTexture("buildings/buildings.png");
            pUnit->UVs = {
                240.f / pUnit->pTexture->getSizef().x, 0.f / pUnit->pTexture->getSizef().y, 
                440.f / pUnit->pTexture->getSizef().x, 200.f / pUnit->pTexture->getSizef().y};
            pUnit->spriteOffsetAndSize = {-40.f / 40.f, -40.f / 40.f, 200.f / 40.f, 200.f / 40.f};
            break;
    }

    if (pUnit->sizeType == eUnitSizeType::BOX)
    {
        int pos[2] = {(int)round(pUnit->position.x), (int)round(pUnit->position.y)};
        for (auto y = pos[1]; y < pUnit->boxSize.y; ++y)
        {
            for (auto x = pos[0]; x < pUnit->boxSize.x; ++x)
            {
                pCollisions[y * m_tiledMap.getWidth() + x] = 0x02;
            }
        }
    }

    if (!pUnitStart) pUnitStart = pUnit;
    else
    {
        pUnitStart->pPrevious = pUnit;
        pUnit->pNext = pUnitStart;
        pUnitStart = pUnit;
    }

    return pUnit;
}
