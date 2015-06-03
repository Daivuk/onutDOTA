#include "Unit.h"
#include "map.h"

Unit::Unit()
    : pChunk(nullptr)
{
}

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

Vector2 Unit::getCenter() const
{
    if (sizeType == eUnitSizeType::BOX)
    {
        return position + Vector2((float)boxSize.x * .5f, (float)boxSize.y * .5f);
    }
    return position;
}
