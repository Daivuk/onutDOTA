#include "Globals.h"

Globals::SUser Globals::user;

Globals::SUser Globals::userFromJson(const rapidjson::Value &json)
{
    SUser ret;

    if (json["token"].IsString()) ret.token = json["token"].GetString();
    if (json["username"].IsString()) ret.username = json["username"].GetString();
    if (json["id"].IsInt64()) ret.id = json["id"].GetInt64();
    if (json["xp"].IsInt64()) ret.xp = json["xp"].GetInt64();
    if (json["level"].IsInt64()) ret.level = json["level"].GetInt64();

    return std::move(ret);
}
