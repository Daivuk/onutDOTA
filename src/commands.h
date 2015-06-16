#pragma once
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

void hookCommands();

enum eCMD : uint8_t
{
    CMD_CHAT_MSG = 1,
    CMD_MOVE_TO,
    CMD_ATTACK_TO,
    CMD_TRIGGER_ABILITY
};


#pragma pack (push)
#pragma pack (1)
struct sCMD_CHAT_MSG
{
    char text[80];
};
#pragma pack (pop)


#pragma pack (push)
#pragma pack (1)
struct sCMD_MOVE_TO
{
    Vector2 mapPos;
};
#pragma pack (pop)


#pragma pack (push)
#pragma pack (1)
struct sCMD_ATTACK_TO
{
    Vector2 mapPos;
};
#pragma pack (pop)


#pragma pack (push)
#pragma pack (1)
struct sCMD_TRIGGER_ABILITY
{
    uint8_t abilityId;
    Vector2 mapPos;
    uint32_t ownerId;
    uint32_t targetId;
};
#pragma pack (pop)
