#pragma once

#include "UnitAnim.h"
#include "List.h"

#define DECAL_DURATION 20

enum eFX : uint8_t
{
    FX_ANIM_BLOOD_A,
    FX_ANIM_BLOOD_B,
    FX_ANIM_BLOOD_C,
    FX_ANIM_BLOOD_D,

    FX_ANIM_MOVE_TO,
    FX_ANIM_ATTACK_TO,

    FX_ANIM_FIRE_BALL,
    FX_ANIM_FIRE_EXPLOSION,

    FX_DECAL_BLOOD_A,
    FX_DECAL_BLOOD_B,
    FX_DECAL_BLOOD_C,
    FX_DECAL_BLOOD_D,

    FX_DECAL_GROUND_MARK,

    FX_COUNT
};

class FX
{
public:
    FX(eFX fxAnim, const Vector2& in_position, float in_angle = 0);

    bool rts_update();
    void render();

    sUnitAnim anim;
    Vector2 position;
    float angle;

    static void init();
    static UnitAnimDef* s_FXAnims[FX_COUNT];
};

class Decal
{
public:
    Decal(eFX fxAnim, const Vector2& in_position, float in_angle = 0, float in_opacity = 1.f, float in_scale = 1.f);

    bool rts_update();
    void render();

    LIST_LINK(Decal) linkMain;

    float life = DECAL_DURATION;
    sUnitAnim anim;
    Vector2 position;
    float angle;
    float opacity = 1.f;
    float scale;
};
