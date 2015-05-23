#pragma once

void hookCommands();

#pragma pack (push)
#pragma pack (1)

#define CMD_CHAT_MSG 1
struct sCMD_CHAT_MSG
{
    char text[80];
};

#pragma pack (pop)
