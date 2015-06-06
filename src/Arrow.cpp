#include "Arrow.h"

static const float ARROW_SPEED = 13.f;

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
            // Damage target
            if (pTarget->damage(pType->damage))
            {
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
