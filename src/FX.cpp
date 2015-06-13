#include "FX.h"

#define FX_FPS 24
#define FX_SIZE {40, 40}
#define FX_OFFSET {-20, -20}
#define FX_ANIM_COMMON(__scale__) "fx/fx.png", FX_SIZE, FX_OFFSET, __scale__, false, FX_FPS
#define FX_DECAL_COMMON(__scale__) "fx/decal.png", FX_SIZE, FX_OFFSET, __scale__, false, FX_FPS
std::vector<int> fxSequence(int row, int col, int count)
{
    static const int COL_COUNT = 12;
    std::vector<int> seq;

    int index = row % COL_COUNT + col * COL_COUNT;
    for (int i = 0; i < count; ++i)
    {
        seq.push_back(index++);
    }

    return std::move(seq);
}

UnitAnimDef* FX::s_FXAnims[FX_COUNT];

void FX::init()
{
    s_FXAnims[FX_ANIM_BLOOD_A] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(0, 0, 6)};
    s_FXAnims[FX_ANIM_BLOOD_B] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(0, 1, 6)};
    s_FXAnims[FX_ANIM_BLOOD_C] = new UnitAnimDef{FX_ANIM_COMMON(2), fxSequence(6, 0, 6)};
    s_FXAnims[FX_ANIM_BLOOD_D] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(6, 1, 6)};

    s_FXAnims[FX_ANIM_MOVE_TO] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(0, 19, 6)};
    s_FXAnims[FX_ANIM_ATTACK_TO] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(6, 19, 6)};

    s_FXAnims[FX_ANIM_FIRE_BALL] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(0, 8, 6)};
    s_FXAnims[FX_ANIM_FIRE_EXPLOSION] = new UnitAnimDef{FX_ANIM_COMMON(1), fxSequence(0, 9, 9)};

    s_FXAnims[FX_DECAL_BLOOD_A] = new UnitAnimDef{FX_DECAL_COMMON(1), {0}};
    s_FXAnims[FX_DECAL_BLOOD_B] = new UnitAnimDef{FX_DECAL_COMMON(1), {1}};
    s_FXAnims[FX_DECAL_BLOOD_C] = new UnitAnimDef{FX_DECAL_COMMON(1), {2}};
    s_FXAnims[FX_DECAL_BLOOD_D] = new UnitAnimDef{FX_DECAL_COMMON(1), {3}};

    s_FXAnims[FX_DECAL_GROUND_MARK] = new UnitAnimDef{FX_DECAL_COMMON(1), {4}};
}

FX::FX(eFX fxAnim, const Vector2& in_position, float in_angle)
    : position(in_position)
    , angle(in_angle)
{
    anim.pAnimDef = s_FXAnims[fxAnim];
}

bool FX::rts_update()
{
    anim.progress += ODT * (float)anim.pAnimDef->fps;
    anim.frame = (int)anim.progress;
    return anim.frame >= anim.pAnimDef->frameCount;
}

void FX::render()
{
    auto &frame = anim.pAnimDef->frames[anim.frame];
    OSB->drawRectWithUVs(anim.pAnimDef->pTexture, {position + frame.offset, frame.size}, frame.UVs);
}

Decal::Decal(eFX fxAnim, const Vector2& in_position, float in_angle, float in_opacity, float in_scale)
    : position(in_position)
    , angle(in_angle)
    , opacity(in_opacity)
    , scale(in_scale)
{
    anim.pAnimDef = FX::s_FXAnims[fxAnim];
}

bool Decal::rts_update()
{
    life -= ODT;
    anim.progress += ODT * (float)anim.pAnimDef->fps;
    anim.frame = (int)anim.progress % anim.pAnimDef->frameCount;
    return life <= 0.f;
}

void Decal::render()
{
    auto &frame = anim.pAnimDef->frames[anim.frame];
    auto color = std::min<float>(life / 2.f, 1.f);
    OSB->drawRectWithUVs(anim.pAnimDef->pTexture, {position + frame.offset * scale, frame.size * scale}, frame.UVs, Color{color * opacity});
}
