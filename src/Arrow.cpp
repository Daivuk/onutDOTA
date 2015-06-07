#include "Arrow.h"
#include "Globals.h"

static const float ARROW_SPEED = 13.f;

void Arrow::onSpawn()
{
    Globals::pMap->playSound(position, Globals::pArrow_spawnSound);
}

void Arrow::render()
{
    if (pTarget)
    {
        Vector2 dir = pTarget->position - position;
        dir.Normalize();
        float angle = DirectX::XMConvertToDegrees(atan2f(dir.y, dir.x));
        OSB->drawSprite(pType->pTexture, position, Color::White, angle, 2.f / 40.f);
    }
}

void Arrow::rts_update()
{
    if (pTarget)
    {
        Vector2 dir = pTarget->position - position;
        float distLeft = dir.Length();
        if (distLeft > 0) dir /= distLeft;
        distLeft -= ODT * ARROW_SPEED;
        if (distLeft <= pTarget->pType->radius)
        {
            // Play impact sound
            Globals::pMap->playSound(position, Globals::pArrow_hit);

            // Show some blood
            if (pTarget->pType->category == eUnitCategory::GROUND)
            {
                Globals::pMap->spawnFX(eFX::FX_ANIM_BLOOD_D, position);
                Globals::pMap->spawnDecal(eFX((int)eFX::FX_DECAL_BLOOD_A + onut::randi(3)), position, 0, .75f);
            }

            // Damage target
            if (pTarget->damage(pType->damage))
            {
                if (pTarget->pType->category == eUnitCategory::GROUND)
                {
                    Globals::pMap->spawnDecal(eFX((int)eFX::FX_DECAL_BLOOD_A + onut::randi(1)), position, 0, .75f, 1.5f);
                }

                // Yay we killed it
                if (pOwner)
                {
                    pOwner->kills++;
                }
            }
            markForDeletion();
            return;
        }
        position = pTarget->position - dir * distLeft;
    }
    else
    {
        markForDeletion();
    }
}
