#pragma once
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

void hookCommands();

enum eCMD : uint8_t
{
    CMD_CHAT_MSG = 1,
    CMD_MOVE_TO,
    CMD_ATTACK_TO
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
