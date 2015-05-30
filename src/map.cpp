#include "Globals.h"
#include "map.h"
#include "Unit.h"
#include "eg.h"

#include "Spawner.h"
#include "Nexus.h"
#include "Waypoint.h"

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
            if (pObject.properties.find("team") != pObject.properties.end())
            {
                try
                {
                    team = std::stoi(pObject.properties["team"]);
                }
                catch (std::exception e)
                {
                    continue;
                }
            }
            Unit *pUnit = nullptr;
            if (pObject.type == "Spawner")
            {
                pUnit = spawn(pObject.position, eUnitType::SPAWNER, team);
                if (pUnit)
                {
                    auto pSpawner = dynamic_cast<Spawner*>(pUnit);
                    try
                    {
                        pSpawner->firstWaypoiuntId = std::stoi(pObject.properties["next"]);
                    }
                    catch (std::exception e) 
                    {
                        pSpawner->firstWaypoiuntId = 0;
                    }
                }
            }
            else if (pObject.type == "Nexus")
            {
                pUnit = spawn(pObject.position, eUnitType::NEXUS, team);
            }
            else if (pObject.type == "MinionWaypoint")
            {
                pUnit = spawn(pObject.position, eUnitType::WAYPOINT, -1);
                if (pUnit)
                {
                    auto pWaypoint = dynamic_cast<Waypoint*>(pUnit);
                    try
                    {
                        pWaypoint->nextWayPointId = std::stoi(pObject.properties["next"]);
                    }
                    catch (std::exception e) 
                    {
                        pWaypoint->nextWayPointId = 0;
                    }
                    pUnit->boxSize = {(LONG)pObject.size.x, (LONG)pObject.size.y};
                }
            }
            if (pUnit)
            {
                pUnit->mapId = pObject.id;
            }
        }
    }

    // Link all way points
    for (auto pUnit : m_units)
    {
        auto pSpawner = dynamic_cast<Spawner*>(pUnit);
        if (!pSpawner) continue;
        pSpawner->pFirstWaypoint = getUnitByMapId(pSpawner->firstWaypoiuntId);
        auto team = pSpawner->team;
        if (pSpawner->pFirstWaypoint)
        {
            Unit *pPrevious = pSpawner;
            auto pWaypoint = dynamic_cast<Waypoint*>(pSpawner->pFirstWaypoint);
            while (pWaypoint)
            {
                pWaypoint->pPrevious[team] = pPrevious;
                auto pNext = getUnitByMapId(pWaypoint->nextWayPointId);
                if (pNext == pPrevious || !pNext)
                {
                    for (auto pOther : m_units)
                    {
                        if (pOther == pWaypoint) continue;
                        auto pOtherWaypoint = dynamic_cast<Waypoint*>(pOther);
                        if (pOtherWaypoint)
                        {
                            if (pWaypoint == getUnitByMapId(pOtherWaypoint->nextWayPointId))
                            {
                                if (pOtherWaypoint != pWaypoint->pPrevious[team])
                                {
                                    pWaypoint->pNext[team] = pOtherWaypoint;
                                    pPrevious = pWaypoint;
                                    pWaypoint = pOtherWaypoint;
                                    break;
                                }
                            }
                        }
                        auto pOtherSpawner = dynamic_cast<Spawner*>(pOther);
                        if (pOtherSpawner)
                        {
                            if (pWaypoint == getUnitByMapId(pOtherSpawner->firstWaypoiuntId))
                            {
                                if (pOtherSpawner != pWaypoint->pPrevious[team])
                                {
                                    pWaypoint->pNext[team] = pOtherSpawner;
                                    pPrevious = pWaypoint;
                                    pWaypoint = nullptr;
                                    break;
                                }
                            }
                        }
                    }
                    continue;
                }
                pWaypoint->pNext[team] = pNext;
                pPrevious = pWaypoint;
                pWaypoint = dynamic_cast<Waypoint*>(pNext);
            }
        }
    }
}

Map::~Map()
{
    delete[] pCollisions;
}

Unit *Map::getUnitByMapId(uint32_t mapId)
{
    for (auto pUnit : m_units)
    {
        if (pUnit->mapId == mapId) return pUnit;
    }
    return nullptr;
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

#if _DEBUG
    for (auto pUnit : m_units)
    {
        auto pSpawner = dynamic_cast<Spawner*>(pUnit);
        if (pSpawner)
        {
            Color color = {1.f, .5f, .5f, 1.f};
            Vector2 offset = {-.25f, -.25f};
            if (pSpawner->team == 1)
            {
                color = {.5f, .5f, 1.f, 1.f};
                offset = {.25f, .25f};
            }
            OPB->begin(onut::ePrimitiveType::LINE_STRIP);
            OPB->draw(pSpawner->position + Vector2((float)pSpawner->boxSize.x * .5f, (float)pSpawner->boxSize.y * .5f) + offset, color);
            auto pNext = pSpawner->pFirstWaypoint;
            while (pNext)
            {
                OPB->draw(pNext->position + Vector2((float)pNext->boxSize.x * .5f, (float)pNext->boxSize.y * .5f) + offset, color);
                auto pWaypoint = dynamic_cast<Waypoint*>(pNext);
                if (pWaypoint)
                {
                    pNext = pWaypoint->pNext[pSpawner->team];
                }
                else
                {
                    break;
                }
            }
            OPB->end();
        }
    }
#endif

    OSB->begin();
    for (auto pUnit : m_units)
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
    for (auto pUnit : m_units)
    {
        pUnit->rts_update();
    }
}

Unit *Map::spawn(const Vector2 &position, eUnitType unitType, int team)
{
    // Alloc a new unit
    Unit *pUnit = nullptr;
    switch (unitType)
    {
        case eUnitType::SPAWNER:
        {
            pUnit = m_unitPool.alloc<Spawner>();
            if (!pUnit) return nullptr;
            pUnit->sizeType = eUnitSizeType::BOX;
            pUnit->boxSize = {3, 3};
            pUnit->pTexture = OGetTexture("buildings/buildings.png");
            pUnit->UVs = {
                240.f / pUnit->pTexture->getSizef().x, 0.f / pUnit->pTexture->getSizef().y, 
                440.f / pUnit->pTexture->getSizef().x, 200.f / pUnit->pTexture->getSizef().y};
            pUnit->spriteOffsetAndSize = {-40.f / 40.f, -40.f / 40.f, 200.f / 40.f, 200.f / 40.f};
            break;
        }
        case eUnitType::NEXUS:
        {
            pUnit = m_unitPool.alloc<Nexus>();
            if (!pUnit) return nullptr;
            pUnit->sizeType = eUnitSizeType::BOX;
            pUnit->boxSize = {4, 4};
            pUnit->pTexture = OGetTexture("buildings/buildings.png");
            pUnit->UVs = {
                0.f / pUnit->pTexture->getSizef().x, 0.f / pUnit->pTexture->getSizef().y, 
                240.f / pUnit->pTexture->getSizef().x, 240.f / pUnit->pTexture->getSizef().y};
            pUnit->spriteOffsetAndSize = {-40.f / 40.f, -40.f / 40.f, 240.f / 40.f, 240.f / 40.f};
            break;
        }
        case eUnitType::WAYPOINT:
        {
            pUnit = m_unitPool.alloc<Waypoint>();
            if (!pUnit) return nullptr;
            pUnit->sizeType = eUnitSizeType::NONE;
            break;
        }
        default:
        {
            return nullptr;
        }
    }

    pUnit->position = position;
    pUnit->type = unitType;
    pUnit->team = team;

    if (pUnit->team == 1)
    {
        pUnit->UVs.y += .5f;
        pUnit->UVs.w += .5f;
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

    // Insert at the right place (We order in Y)
    auto it = m_units.begin();
    for (; it != m_units.end(); ++it)
    {
        auto pOther = *it;
        if (pUnit->position.y < pOther->position.y)
        {
            m_units.insert(it, pUnit);
            break;
        }
    }
    if (it == m_units.end())
    {
        m_units.push_back(pUnit);
    }

    return pUnit;
}
