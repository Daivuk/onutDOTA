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
    } myUser;

    static void setMyUser(const SUser &user);

    static SUser userFromJson(const rapidjson::Value &json);

    static onut::UIControl *pUIHeader;
};
