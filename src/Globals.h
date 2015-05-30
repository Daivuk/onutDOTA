#pragma once
#include <string>
#include <cinttypes>
#include "rapidjson/document.h"
#include "onut.h"
#include "map.h"
#include <unordered_map>

static const int BALT_LEFT = 0x10000000;
static const int BALT_RIGHT = 0x20000000;
static const int BALT_UP = 0x40000000;
static const int BALT_DOWN = 0x80000000;

static const int BALT_IDLE = 0x00000001;
static const int BALT_WALK = 0x00000002;
static const int BALT_ATTACK = 0x00000004;

static const int BALT_SHIELD = 0x01000000;

static const int BALT_IDLE_FPS = 8;
static const int BALT_WALK_FPS = 8;
static const int BALT_ATTACK_FPS = 24;

struct sAnimDef
{
    bool hFlip;
    int fps;
    std::vector<int> frames;
};

static const std::unordered_map<int, sAnimDef> baltAnimsDefs = {
    {BALT_DOWN | BALT_IDLE, {false, BALT_IDLE_FPS, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {BALT_DOWN | BALT_WALK, {false, BALT_WALK_FPS, {2, 4, 3, 5}}},
    {BALT_DOWN | BALT_ATTACK, {false, BALT_ATTACK_FPS, {6, 2, 7}}},

    {BALT_UP | BALT_IDLE, {false, BALT_IDLE_FPS, {8}}},
    {BALT_UP | BALT_WALK, {false, BALT_WALK_FPS, {10, 13, 11, 12}}},
    {BALT_UP | BALT_ATTACK, {false, BALT_ATTACK_FPS, {14, 10, 15}}},

    {BALT_LEFT | BALT_IDLE, {false, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}}},
    {BALT_LEFT | BALT_WALK, {false, BALT_WALK_FPS, {18, 16, 19, 16}}},
    {BALT_LEFT | BALT_ATTACK, {false, BALT_ATTACK_FPS, {20, 19, 21}}},

    {BALT_RIGHT | BALT_IDLE, {true, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}}},
    {BALT_RIGHT | BALT_WALK, {true, BALT_WALK_FPS, {18, 16, 19, 16}}},
    {BALT_RIGHT | BALT_ATTACK, {true, BALT_ATTACK_FPS, {20, 19, 21}}},

    {BALT_DOWN | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {32, 32, 32, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32}}},
    {BALT_DOWN | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {34, 36, 35, 37}}},
    {BALT_DOWN | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {34, 34, 39}}},

    {BALT_UP | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {9}}},
    {BALT_UP | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {22, 29, 23, 28}}},
    {BALT_UP | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {30, 22, 31}}},

    {BALT_LEFT | BALT_IDLE | BALT_SHIELD, {false, BALT_IDLE_FPS, {24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24}}},
    {BALT_LEFT | BALT_WALK | BALT_SHIELD, {false, BALT_WALK_FPS, {18, 24, 27, 24}}},
    {BALT_LEFT | BALT_ATTACK | BALT_SHIELD, {false, BALT_ATTACK_FPS, {27, 27, 27}}},

    {BALT_RIGHT | BALT_IDLE | BALT_SHIELD, {true, BALT_IDLE_FPS, {24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24}}},
    {BALT_RIGHT | BALT_WALK | BALT_SHIELD, {true, BALT_WALK_FPS, {18, 24, 27, 24}}},
    {BALT_RIGHT | BALT_ATTACK | BALT_SHIELD, {true, BALT_ATTACK_FPS, {27, 27, 27}}},
};

struct sAnimFrame
{
    Vector2 offset;
    Vector2 size;
    Vector4 UVs;
};

struct sAnimRes
{
    std::vector<sAnimFrame> frames;
    const sAnimDef *pAnimDef;
};

class Globals
{
public:
    static void init();

    static struct SUser
    {
        bool connected = false;
        std::string token;
        int64_t id;
        std::string username;
        int64_t level;
        int64_t xp;
        int team;
        std::string ipPort;
    } myUser;

    static struct SGame
    {
        int64_t id;
        uint32_t seed;
        std::vector<SUser> users;
        std::string status;
    } myGame;

    static const SUser *getUserById(const SGame &game, uint64_t userId);

    static void setMyUser(const SUser &user);

    static bool userFromJson(SUser &user, const rapidjson::Value &json);
    static bool gameFromJson(SGame &game, const rapidjson::Value &json);

    static onut::UIControl *pUIHeader;
    static onut::RTS *pRTS;
    static Map *pMap;

    static std::unordered_map<int, std::vector<sAnimRes>> baltAnimsResources;
};
