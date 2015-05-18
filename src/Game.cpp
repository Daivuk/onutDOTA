#include "Game.h"
#include "Globals.h"
#include "commands.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Game::Game()
{
    pUIScreen = new onut::UIControl("../../assets/ui/game.json");
    pUIScreen->retain();
    pChat = pUIScreen->getChild<onut::UITextBox>("txtChat");
    pChat->retain();
    pChatContainer = pUIScreen->getChild("chatContainer");
    pChatContainer->retain();
    pChatMsgTemplate = pUIScreen->getChild<onut::UILabel>("lblChatMsg");
    pChatMsgTemplate->retain();
    pChatMsgTemplate->remove();

    hookButtonSounds(pUIScreen);
    hookCommands();

    Globals::pRTS->retain();

    pChat->onTextChanged = [=](onut::UITextBox*, const onut::UITextBoxEvent&)
    {
        if (!pChat->textComponent.text.empty())
        {
            // Send text message!
            auto size = std::min<>(size_t(79), pChat->textComponent.text.size());
            sCMD_CHAT_MSG msg;
            memcpy(msg.text, pChat->textComponent.text.c_str(), size);
            msg.text[size] = '\0';
            Globals::pRTS->sendCommand(CMD_CHAT_MSG, &msg);
        }
    };
    pChat->onGainFocus = [=](onut::UIControl*)
    {
        pChat->isVisible = true;
    };
    pChat->onLoseFocus = [=](onut::UIControl*)
    {
        pChat->isVisible = false;
        pChat->textComponent.text.clear();
    };
}

Game::~Game()
{
    pChatMsgTemplate->release();
    pChatContainer->release();
    pChat->release();
    pUIScreen->release();
    Globals::pRTS->release();
}

void Game::update()
{
    auto updateFrames = Globals::pRTS->update();
    while (updateFrames--)
    {
    }

    // Show current turn in debug
#if _DEBUG
    pUIScreen->getChild<onut::UILabel>("lblTurn")->textComponent.text = "Turn: " + std::to_string(Globals::pRTS->getTurn());
#endif
}

void Game::render()
{
}

void Game::enter()
{
    Globals::pRTS->start();
}

void Game::showChat()
{
    if (pChat->isVisible) return;
    g_pUIContext->clearState();
    g_pUIContext->focus(pChat);
}

void Game::onChatMessage(const std::string &msg)
{
    auto pNewChatMsg = dynamic_cast<onut::UILabel*>(pChatMsgTemplate->copy());
    for (auto pChatMsg : pChatContainer->getChildren())
    {
        pChatMsg->rect.position.y -= pNewChatMsg->rect.size.y;
    }
    pNewChatMsg->textComponent.text = msg;
    pChatContainer->add(pNewChatMsg);
}
