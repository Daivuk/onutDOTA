#include "Globals.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Globals::SUser Globals::myUser;
onut::UIControl *Globals::pUIHeader = nullptr;

void Globals::init()
{
    auto pHeader = new onut::UIControl("../../assets/ui/header.json");
    pUIHeader = pHeader->getChild("header");
    pUIHeader->retain();
    pHeader->release();
    hookButtonSounds(pUIHeader);
}

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

void Globals::setMyUser(const SUser &user)
{
    myUser = user;
    pUIHeader->getChild<onut::UILabel>("lblUsername")->textComponent.text = myUser.username;
    pUIHeader->getChild<onut::UILabel>("lblLevel")->textComponent.text = "LVL: " + std::to_string(myUser.level);
    pUIHeader->getChild<onut::UILabel>("lblXP")->textComponent.text = "XP: " + std::to_string(myUser.xp);
    pUIHeader->getChild<onut::UILabel>("lblGold")->textComponent.text = "Gold: 0";
}
