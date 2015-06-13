#include "FallingFireBall.h"
#include "Globals.h"

void FallingFireBall::onSpawn()
{
    fromPos = position;
    toPos = position - FIRE_BALL_START_OFFSET;
    Globals::pMap->playSound(fromPos, Globals::pFireball_spawn, .5f);

    auto dir = toPos - (fromPos + Vector2{0, -FIRE_BACLL_START_HEIGHT});
    dir.Normalize();
    angle = DirectX::XMConvertToDegrees(atan2f(dir.y, dir.x));
}

void FallingFireBall::render()
{
    position = Vector2::Lerp(fromPos, toPos, fallingProgress / FIRE_BALL_DURATION);
    float z = FIRE_BACLL_START_HEIGHT * (1 - fallingProgress / FIRE_BALL_DURATION);

    auto &frame = anim.pAnimDef->frames[anim.frame];
    auto UV = frame.UVs;
    OSB->drawSpriteWithUVs(anim.pAnimDef->pTexture, {position.x, position.y - z}, UV, Color::White, angle, 1.f / 40.f);
}

void FallingFireBall::rts_update()
{
    // Update animation
    anim.progress += ODT * (float)anim.pAnimDef->fps;
    anim.frame = (int)anim.progress % anim.pAnimDef->frameCount;

    fallingProgress += ODT;
    if (fallingProgress >= FIRE_BALL_DURATION)
    {
        Globals::pMap->spawnDecal(eFX::FX_DECAL_GROUND_MARK, toPos, 0, .5f);
        Globals::pMap->spawnFX(eFX::FX_ANIM_FIRE_EXPLOSION, toPos);
        Globals::pMap->playSound(toPos, Globals::pFireball_hit);
        Globals::pMap->splashDamage(toPos, pType->damage, pType->damageRadius, pOwner);
        markForDeletion();
    }
}
