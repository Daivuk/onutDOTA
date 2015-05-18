#include "commands.h"
#include "onut.h"
#include "Globals.h"
#include "View.h"
#include "Game.h"

extern View *g_pCurrentView;

const Globals::SUser *getUserFromPeer(onut::RTSPeer *pPeer)
{
    if (!pPeer) return Globals::getUserById(Globals::myGame, Globals::myUser.id);
    return Globals::getUserById(Globals::myGame, pPeer->getPlayerId());
}

void hookCommands()
{
    Globals::pRTS->registerCommand(CMD_CHAT_MSG, sizeof(sCMD_CHAT_MSG), 
                                   [](void *pData, onut::RTSPeer *pPeer)
    {
        auto pUser = getUserFromPeer(pPeer);
        if (!pUser) return;
        auto pMsg = (sCMD_CHAT_MSG*)pData;

        auto pGame = dynamic_cast<Game*>(g_pCurrentView);
        
        if (pGame)
        {
            if (pUser->team == 0)
            {
                pGame->onChatMessage("^933" + pUser->username + ": ^999" + pMsg->text);
            }
            else
            {
                pGame->onChatMessage("^339" + pUser->username + ": ^999" + pMsg->text);
            }
        }
    });
}
