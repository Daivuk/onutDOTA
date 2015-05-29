#include "Unit.h"

void Unit::render()
{
    if (type == eUnitType::NONE) return;

    OSB->drawRectWithUVs(pTexture,
        {position.x + spriteOffsetAndSize.x, position.y + spriteOffsetAndSize.y, spriteOffsetAndSize.z, spriteOffsetAndSize.w},
        UVs);
}

void Unit::rts_update()
{
    if (type == eUnitType::NONE) return;
}
