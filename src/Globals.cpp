#include "Globals.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Globals::SUser Globals::myUser;
Globals::SGame Globals::myGame;
onut::UIControl *Globals::pUIHeader = nullptr;
onut::RTS *Globals::pRTS = nullptr;

void Globals::init()
{
    auto pHeader = new onut::UIControl("../../assets/ui/header.json");
    pUIHeader = pHeader->getChild("header");
    pUIHeader->retain();
    pHeader->release();
    hookButtonSounds(pUIHeader);
}

bool Globals::userFromJson(SUser &user, const rapidjson::Value &json)
{
    if (!json.IsObject()) return false;

    if (json["token"].IsString()) user.token = json["token"].GetString();
    if (json["username"].IsString()) user.username = json["username"].GetString();
    if (json["id"].IsInt64()) user.id = json["id"].GetInt64();
    if (json["xp"].IsInt64()) user.xp = json["xp"].GetInt64();
    if (json["level"].IsInt64()) user.level = json["level"].GetInt64();
    if (json["teamId"].IsInt()) user.team = json["teamId"].GetInt();
    user.ipPort.clear();
    if (json["ipPort"].IsString())
    {
        auto ipPort = json["ipPort"].GetString();
        if (ipPort != "null")
        {
            user.ipPort = ipPort;
        }
    }

    return true;
}

bool Globals::gameFromJson(SGame &game, const rapidjson::Value &jsonData)
{
    game = {0};

    // Parse json and validate
    if (!jsonData.IsObject())
    {
        return false;
    }
    const auto &jsonGame = jsonData["game"];
    const auto &jsonUsers = jsonData["users"];
    if (!jsonGame.IsObject())
    {
        return false;
    }
    if (!jsonUsers.IsArray())
    {
        return false;
    }

    // Parse game
    const auto &jsonGameId = jsonGame["id"];
    if (!jsonGameId.IsInt64())
    {
        return false;
    }
    game.id = jsonGameId.GetInt64();
    const auto &jsonGameSeed = jsonGame["seed"];
    if (!jsonGameSeed.IsUint())
    {
        return false;
    }
    game.seed = jsonGameSeed.GetUint();

    // Parse users
    for (decltype(jsonUsers.Size()) i = 0; i < jsonUsers.Size(); ++i)
    {
        const auto &jsonUser = jsonUsers[i];
        SUser user;
        if (!userFromJson(user, jsonUser))
        {
            return false;
        }
        game.users.push_back(user);
    }

    return true;
}

void Globals::setMyUser(const SUser &user)
{
    myUser = user;
    pUIHeader->getChild<onut::UILabel>("lblUsername")->textComponent.text = myUser.username;
    pUIHeader->getChild<onut::UILabel>("lblLevel")->textComponent.text = "LVL: " + std::to_string(myUser.level);
    pUIHeader->getChild<onut::UILabel>("lblXP")->textComponent.text = "XP: " + std::to_string(myUser.xp);
    pUIHeader->getChild<onut::UILabel>("lblGold")->textComponent.text = "Gold: 0";
}
