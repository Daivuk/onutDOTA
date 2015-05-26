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
    // Define our camera transform matrix
    auto camX = round(m_cameraPos.x * 40 - OScreenWf * .5f);
    auto camY = round(m_cameraPos.y * 40 - OScreenHf * .5f);
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
}
