#pragma once
#include <string>
#include <cinttypes>
#include "rapidjson/document.h"

class Globals
{
public:
    static struct SUser
    {
        bool connected = false;
        std::string token;
        int64_t id;
        std::string username;
        int64_t level;
        int64_t xp;
    } user;

    static SUser userFromJson(const rapidjson::Value &json);
};
