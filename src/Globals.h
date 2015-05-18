#pragma once
#include <string>
#include <cinttypes>
#include "rapidjson/document.h"
#include "onut.h"

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
};
