#include "Globals.h"
#include "FX.h"

#include "Spawner.h"
#include "Nexus.h"
#include "Waypoint.h"
#include "Minion.h"
#include "Arrow.h"
#include "Tower.h"
#include "Gibs.h"
#include "Hero.h"
#include "SpawnPoint.h"
#include "FallingFireBall.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Globals::SUser Globals::myUser;
Globals::SGame Globals::myGame;
onut::UIControl *Globals::pUIHeader = nullptr;
onut::RTS *Globals::pRTS = nullptr;
Map *Globals::pMap = nullptr;
uint32_t Globals::rts_frame = 0;

std::unordered_map<eUnitType, sUnitType> Globals::unitTypes;
std::unordered_map<std::string, eUnitType> Globals::unitTypesByName;

std::unordered_map<std::string, std::string> Globals::gameValues;

OSound *Globals::pArrow_spawnSound = nullptr;
OSound *Globals::pArrow_hit = nullptr;
OSound *Globals::pFireball_spawn = nullptr;
OSound *Globals::pFireball_hit = nullptr;

int Globals::getInt(const std::string& key, int defaultValue)
{
    auto it = gameValues.find(key);
    if (it != gameValues.end())
    {
        try
        {
            auto ret = std::stoi(it->second);
            return ret;
        }
        catch (std::exception e)
        {
            return defaultValue;
        }
    }
    gameValues[key] = std::to_string(defaultValue);
    return defaultValue;
}

float Globals::getFloat(const std::string& key, float defaultValue)
{
    auto it = gameValues.find(key);
    if (it != gameValues.end())
    {
        try
        {
            auto ret = std::stof(it->second);
            return ret;
        }
        catch (std::exception e)
        {
            return defaultValue;
        }
    }
    gameValues[key] = std::to_string(defaultValue);
    return defaultValue;
}

void Globals::init()
{
    // FX
    FX::init();

    // Define unit types
    {
        sUnitType u;
        u.typeName = "Spawner";
        u.screenName = "Spawner";
        u.category = eUnitCategory::BUILDLING;
        u.sizeType = eUnitSizeType::BOX;
        u.boxSize = {3, 3};
        u.yOffset = 3;
        u.health = getInt("spawner_health", 500);
        u.armor = getInt("spawner_armor", 5);
        u.visionRange = getFloat("spawner_vision_range", 10);
        u.alertRange = getFloat("spawner_alert_range", 8);
        u.anims[BALT_DOWN | BALT_IDLE] = new UnitAnimDef{"buildings/buildings.png", {-40.f, -40.f}, {200.f, 200.f}, false, 0, {{240.f, 0.f, 200.f, 200.f}}};
        u.anims[BALT_UP | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_LEFT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_RIGHT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.pFactory = new UnitFactory<Spawner>();
        unitTypes[eUnitType::SPAWNER] = u;
    }
    {
        sUnitType u;
        u.typeName = "Tower";
        u.screenName = "Tower";
        u.category = eUnitCategory::BUILDLING;
        u.sizeType = eUnitSizeType::BOX;
        u.boxSize = {2, 2};
        u.yOffset = 2;
        u.health = 300;
        u.armor = 3;
        u.visionRange = 10;
        u.alertRange = 8;
        u.attackRange = 8;
        u.attackType = eUnitAttackType::PROJECTILE;
        u.projectileUnitType = eUnitType::ARROW;
        u.attackCoolDown = .75f;
        u.attackDelay = 0.01f;
        u.anims[BALT_DOWN | BALT_IDLE] = new UnitAnimDef{"buildings/buildings.png", {0, -80.f}, {120.f, 200.f}, false, 120, {{480.f, 0.f, 120.f, 200.f}}};
        u.anims[BALT_UP | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_LEFT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_RIGHT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_DOWN | BALT_ATTACK] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_UP | BALT_ATTACK] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_LEFT | BALT_ATTACK] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_RIGHT | BALT_ATTACK] = u.anims[BALT_DOWN | BALT_IDLE];
        u.pFactory = new UnitFactory<Tower>();
        unitTypes[eUnitType::TOWER] = u;
    }
    {
        sUnitType u;
        u.typeName = "Nexus";
        u.screenName = "Nexus";
        u.category = eUnitCategory::BUILDLING;
        u.sizeType = eUnitSizeType::BOX;
        u.boxSize = {4, 4};
        u.yOffset = 4;
        u.health = 2000;
        u.armor = 5;
        u.visionRange = 10;
        u.alertRange = 8;
        u.anims[BALT_DOWN | BALT_IDLE] = new UnitAnimDef{"buildings/buildings.png", {-40.f, -40.f}, {240.f, 240.f}, false, 0, {{0, 0, 240.f, 240.f}}};
        u.anims[BALT_UP | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_LEFT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.anims[BALT_RIGHT | BALT_IDLE] = u.anims[BALT_DOWN | BALT_IDLE];
        u.pFactory = new UnitFactory<Nexus>();
        unitTypes[eUnitType::NEXUS] = u;
    }
    {
        sUnitType u;
        u.typeName = "MinionWaypoint";
        u.category = eUnitCategory::NONE;
        u.sizeType = eUnitSizeType::BOX;
        u.boxSize = {2, 2};
        u.pFactory = new UnitFactory<Waypoint>();
        unitTypes[eUnitType::WAYPOINT] = u;
    }
    {
        sUnitType u;
        u.typeName = "Minion";
        u.screenName = "Minion";
        u.category = eUnitCategory::GROUND;
        u.sizeType = eUnitSizeType::RADIUS;
        u.radius = .25f;
        u.health = 40;
        u.visionRange = 7;
        u.alertRange = 6;
        u.attackRange = 5;
        u.moveSpeed = 2;
        u.attackType = eUnitAttackType::PROJECTILE;
        u.projectileUnitType = eUnitType::ARROW;
        u.attackCoolDown = 1.125f;
        u.attackDelay = .08f;
        
        static const int BALT_IDLE_FPS = 8;
        static const int BALT_WALK_FPS = 8;
        static const int BALT_ATTACK_FPS = 24;
        static const float MINION_SCALE = 2.f;

        u.anims[BALT_DOWN | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_IDLE_FPS, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        u.anims[BALT_DOWN | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_WALK_FPS, {2, 4, 3, 5}};
        u.anims[BALT_DOWN | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_ATTACK_FPS, {6, 2, 7}};

        u.anims[BALT_UP | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_IDLE_FPS, {8}};
        u.anims[BALT_UP | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_WALK_FPS, {10, 13, 11, 12}};
        u.anims[BALT_UP | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_ATTACK_FPS, {14, 10, 15}};

        u.anims[BALT_LEFT | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}};
        u.anims[BALT_LEFT | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_WALK_FPS, {18, 16, 19, 16}};
        u.anims[BALT_LEFT | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, false, BALT_ATTACK_FPS, {20, 19, 21}};

        u.anims[BALT_RIGHT | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, true, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}};
        u.anims[BALT_RIGHT | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, true, BALT_WALK_FPS, {18, 16, 19, 16}};
        u.anims[BALT_RIGHT | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, MINION_SCALE, true, BALT_ATTACK_FPS, {20, 19, 21}};

        u.pFactory = new UnitFactory<Minion>();
        unitTypes[eUnitType::MINION] = u;
    }
    {
        sUnitType u;
        u.typeName = "Arrow";
        u.category = eUnitCategory::PROJECTILE;
        u.pFactory = new UnitFactory<Arrow>();
        u.pTexture = OGetTexture("minions/arrow.png");
        u.damage = 5.f;
        unitTypes[eUnitType::ARROW] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {0, 24, 10, 13});
        unitTypes[eUnitType::GIBS_HEAD] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {7, 0, 6, 9});
        unitTypes[eUnitType::GIBS_LARM] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {0, 15, 11, 9});
        unitTypes[eUnitType::GIBS_TORSO] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {0, 0, 7, 8});
        unitTypes[eUnitType::GIBS_RARM] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {0, 8, 6, 7});
        unitTypes[eUnitType::GIBS_LFOOT] = u;
    }
    {
        sUnitType u;
        u.typeName = "Gibs";
        u.pFactory = new UnitFactory<Gibs>();
        u.pTexture = OGetTexture("fx/gibs.png");
        u.UVs = OUVS(u.pTexture, {0, 9, 6, 6});
        unitTypes[eUnitType::GIBS_RFOOT] = u;
    }
    {
        sUnitType u;
        u.typeName = "Hero";
        u.screenName = "Hero";
        u.category = eUnitCategory::GROUND;
        u.sizeType = eUnitSizeType::RADIUS;
        u.radius = .35f;
        u.health = 150;
        u.visionRange = 10;
        u.alertRange = 9;
        u.attackRange = 8;
        u.moveSpeed = 4;
        u.attackType = eUnitAttackType::PROJECTILE;
        u.projectileUnitType = eUnitType::ARROW;
        u.attackCoolDown = 1.125f;
        u.attackDelay = .08f;

        static const int BALT_IDLE_FPS = 12;
        static const int BALT_WALK_FPS = 12;
        static const int BALT_ATTACK_FPS = 24;
        static const float HERO_SCALE = 3.f;

        u.anims[BALT_DOWN | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_IDLE_FPS, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        u.anims[BALT_DOWN | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_WALK_FPS, {2, 4, 3, 5}};
        u.anims[BALT_DOWN | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_ATTACK_FPS, {6, 2, 7}};

        u.anims[BALT_UP | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_IDLE_FPS, {8}};
        u.anims[BALT_UP | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_WALK_FPS, {10, 13, 11, 12}};
        u.anims[BALT_UP | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_ATTACK_FPS, {14, 10, 15}};

        u.anims[BALT_LEFT | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}};
        u.anims[BALT_LEFT | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_WALK_FPS, {18, 16, 19, 16}};
        u.anims[BALT_LEFT | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, false, BALT_ATTACK_FPS, {20, 19, 21}};

        u.anims[BALT_RIGHT | BALT_IDLE] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, true, BALT_IDLE_FPS, {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}};
        u.anims[BALT_RIGHT | BALT_WALK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, true, BALT_WALK_FPS, {18, 16, 19, 16}};
        u.anims[BALT_RIGHT | BALT_ATTACK] = new UnitAnimDef{"minions/beggarPlateArmor.png", {16, 24}, {-8, -18}, HERO_SCALE, true, BALT_ATTACK_FPS, {20, 19, 21}};

        u.pFactory = new UnitFactory<Hero>();
        unitTypes[eUnitType::HERO] = u;
    }
    {
        sUnitType u;
        u.typeName = "SpawnPoint";
        u.category = eUnitCategory::NONE;
        u.sizeType = eUnitSizeType::BOX;
        u.boxSize = {1, 1};
        u.pFactory = new UnitFactory<SpawnPoint>();
        unitTypes[eUnitType::SPAWN_POINT] = u;
    }
    {
        sUnitType u;
        u.category = eUnitCategory::PROJECTILE;
        u.pFactory = new UnitFactory<FallingFireBall>();
        u.damage = 10.f;
        u.damageRadius = 1.0f;
        u.anims[BALT_DOWN | BALT_IDLE] = FX::s_FXAnims[FX_ANIM_FIRE_BALL];
        unitTypes[eUnitType::FALLING_FIRE_BALL] = u;
    }

    // Sounds
    pArrow_spawnSound = OGetSound("arrow.wav");
    pArrow_spawnSound->setMaxInstance(4);
    pArrow_hit = OGetSound("arrowHit.wav");
    pArrow_hit->setMaxInstance(2);
    pFireball_spawn = OGetSound("fireball.wav");
    pFireball_spawn->setMaxInstance(5);
    pFireball_hit = OGetSound("fireballImpact.wav");
    pFireball_hit->setMaxInstance(5);

    for (auto &unitType : unitTypes)
    {
        unitTypesByName[unitType.second.typeName] = unitType.first;
    }

    auto pHeader = new onut::UIControl("../../assets/ui/header.json");
    pUIHeader = pHeader->getChild("header");
    pUIHeader->retain();
    pHeader->release();
    hookButtonSounds(pUIHeader);
}

UnitAnimDef::UnitAnimDef(const std::string &texture, const POINT &spriteSize, const Vector2 &offset, float scale, bool hFlip, int in_fps, std::vector<int> in_frames)
    : frameCount(static_cast<int>(in_frames.size()))
    , fps(in_fps)
{
    pTexture = OGetTexture(texture.c_str());
    assert(pTexture);

    int colCount = pTexture->getSize().x / spriteSize.x;
    frames = new sAnimFrame[frameCount];

    int frameIndex = 0;
    float texSizeX = pTexture->getSizef().x;
    float texSizeY = pTexture->getSizef().y;
    for (auto frameId : in_frames)
    {
        auto &frame = frames[frameIndex];
        frame.offset = offset * scale / 40.f;
        frame.size = Vector2{(float)spriteSize.x * scale, (float)spriteSize.y * scale} / 40.f;
        frame.UVs.x = ((float)(frameId % colCount) * (float)spriteSize.x) / texSizeX;
        frame.UVs.y = ((float)(frameId / colCount) * (float)spriteSize.y) / texSizeY;
        frame.UVs.z = ((float)(frameId % colCount + 1) * (float)spriteSize.x) / texSizeX;
        frame.UVs.w = ((float)(frameId / colCount + 1) * (float)spriteSize.y) / texSizeY;
        if (hFlip)
        {
            std::swap(frame.UVs.x, frame.UVs.z);
        }
        ++frameIndex;
    }
}

UnitAnimDef::UnitAnimDef(const std::string &texture, const Vector2 &offset, const Vector2 &size, bool hFlip, int in_fps, std::vector<Vector4> in_frames)
    : frameCount(static_cast<int>(in_frames.size()))
    , fps(in_fps)
{
    pTexture = OGetTexture(texture.c_str());
    assert(pTexture);

    frames = new sAnimFrame[frameCount];

    int frameIndex = 0;
    float texSizeX = pTexture->getSizef().x;
    float texSizeY = pTexture->getSizef().y;
    for (auto &frameUVs : in_frames)
    {
        auto &frame = frames[frameIndex];
        frame.offset = offset / 40.f;
        frame.size = size / 40.f;
        frame.UVs = frameUVs;
        frame.UVs.x /= texSizeX;
        frame.UVs.y /= texSizeY;
        frame.UVs.z /= texSizeX;
        frame.UVs.w /= texSizeY;
        frame.UVs.z += frame.UVs.x;
        frame.UVs.w += frame.UVs.y;
        if (hFlip)
        {
            std::swap(frame.UVs.x, frame.UVs.z);
        }
        ++frameIndex;
    }
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

    const auto &jsonGameStatus = jsonGame["status"];
    if (!jsonGameStatus.IsString())
    {
        return false;
    }
    game.status = jsonGameStatus.GetString();

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

const Globals::SUser *Globals::getUserById(const SGame &game, uint64_t userId)
{
    for (auto &user : game.users)
    {
        if (user.id == userId)
        {
            return &user;
        }
    }
    return nullptr;
}
