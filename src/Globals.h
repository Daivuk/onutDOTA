#pragma once
#include <string>
#include <cinttypes>
#include "rapidjson/document.h"
#include "UnitAnim.h"
#include "map.h"
#include <unordered_map>
#include "Unit.h"

#define VERSION 1

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
        Unit *pUnit = nullptr; // Controlling unit. His hero
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

    static uint32_t rts_frame;

    static std::unordered_map<eUnitType, sUnitType> unitTypes;
    static std::unordered_map<std::string, eUnitType> unitTypesByName;

    static OSound *pArrow_spawnSound;
    static OSound *pArrow_hit;
    static OSound *pFireball_spawn;
    static OSound *pFireball_hit;

    static std::unordered_map<std::string, std::string> gameValues;
    static int getInt(const std::string& key, int defaultValue);
    static float getFloat(const std::string& key, float defaultValue);
};
