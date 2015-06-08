#include "Gibs.h"
#include "Globals.h"

void Gibs::render()
{
    OSB->drawSpriteWithUVs(pType->pTexture, position + Vector2{0, yOffset}, pType->UVs, std::min<float>(1.f, life / 2.f), angle, 2.f / 40.f);
}

void Gibs::rts_update()
{
    life -= ODT;
    if (state < 2)
    {
        angle += angleSpeed * ODT;
        position += Vector2(velocity) * ODT;
        yOffset += velocity.z * ODT;
        velocity.z += 8.f * ODT;
        if (yOffset >= 0.f)
        {
            ++state;
            yOffset = -yOffset * .3f;
            velocity *= .3f;
            velocity.z = -velocity.z;
            angleSpeed *= .5f;
            Globals::pMap->spawnDecal(eFX((int)eFX::FX_DECAL_BLOOD_A + onut::randi(3)), position, 0, .75f);
        }
    }
    if (life < 0) markForDeletion();
}

void Gibs::onSpawn()
{
    switch (type)
    {
        case eUnitType::GIBS_HEAD:
            yOffset = -7.f / 40.f;
            velocity = onut::rand3f({-2, -2, -2}, {2, 2, -5});
            break;
        case eUnitType::GIBS_LARM:
            yOffset = -5.f / 40.f;
            velocity = onut::rand3f({-4, -2, -1}, {-1, 2, -2});
            break;
        case eUnitType::GIBS_TORSO:
            yOffset = -4.f / 40.f;
            velocity = onut::rand3f({-2, -2, -1}, {2, 2, -2});
            break;
        case eUnitType::GIBS_RARM:
            yOffset = -5.f / 40.f;
            velocity = onut::rand3f({4, -2, -1}, {1, 2, -2});
            break;
        case eUnitType::GIBS_LFOOT:
            yOffset = -1.f / 40.f;
            velocity = onut::rand3f({-2, -1, 0}, {0, 1, -1});
            break;
        case eUnitType::GIBS_RFOOT:
            yOffset = -1.f / 40.f;
            velocity = onut::rand3f({0, -1, 0}, {2, 1, -1});
            break;
    }
}
