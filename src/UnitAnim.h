#pragma once

#include "onut.h"

#define BALT_LEFT 0x10000000
#define BALT_RIGHT 0x20000000
#define BALT_UP 0x40000000
#define BALT_DOWN 0x80000000

#define BALT_IDLE 0x00000001
#define BALT_WALK 0x00000002
#define BALT_ATTACK 0x00000004

//static const int BALT_SHIELD = 0x01000000;


//{BALT_DOWN | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {32, 32, 32, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32}}},
//{BALT_DOWN | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {34, 36, 35, 37}}},
//{BALT_DOWN | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {34, 34, 39}}},

//{BALT_UP | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {9}}},
//{BALT_UP | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {22, 29, 23, 28}}},
//{BALT_UP | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {30, 22, 31}}},

//{BALT_LEFT | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24}}},
//{BALT_LEFT | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {18, 24, 27, 24}}},
//{BALT_LEFT | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {27, 27, 27}}},

//{BALT_RIGHT | BALT_IDLE | BALT_SHIELD, {true, BALT_IDLE_FPS, {24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24}}},
//{BALT_RIGHT | BALT_WALK | BALT_SHIELD, {true, BALT_WALK_FPS, {18, 24, 27, 24}}},
//{BALT_RIGHT | BALT_ATTACK | BALT_SHIELD, {true, BALT_ATTACK_FPS, {27, 27, 27}}},

struct sAnimFrame
{
    Vector2 offset;
    Vector2 size;
    Vector4 UVs;
};

class UnitAnimDef
{
public:
    UnitAnimDef(const std::string &texture, const POINT &spriteSize, const Vector2 &offset, float scale, bool hFlip, int in_fps, std::vector<int> in_frames);
    UnitAnimDef(const std::string &texture, const Vector2 &offset, const Vector2 &size, bool hFlip, int in_fps, std::vector<Vector4> in_frames);

    int frameCount;
    int fps;
    sAnimFrame *frames;
    OTexture *pTexture;
};

struct sUnitAnim
{
    UnitAnimDef *pAnimDef;
    int frame = 0;
    float progress = 0.f;
};
