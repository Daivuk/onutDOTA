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
    for (auto pChatMsg : pChatContainer->getChildren())
    {
        auto pCharFader = (ChatFader*)pChatMsg->pUserData;
        pCharFader->release();
    }
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
        // Update avatars
        for (auto &user : Globals::myGame.users)
        {
            auto &avatar = avatarsByPlayerIds[user.id];
            auto dist = Vector2::Distance(avatar.pos, avatar.targetPos);
            dist -= ODT * 400.f;
            if (dist < 0) dist = 0;
            auto dir = avatar.pos - avatar.targetPos;
            dir.Normalize();
            avatar.pos = avatar.targetPos + dir * dist;
        }
    }

    updateChats();

    // Show current turn in debug
#if _DEBUG
    pUIScreen->getChild<onut::UILabel>("lblTurn")->textComponent.text = "Turn: " + std::to_string(Globals::pRTS->getTurn());
#endif

    // User inputs
    if (OInput->isStateJustDown(DIK_MOUSEB2))
    {
        sCMD_MOVE_AVATAR cmd;
        cmd.x = OMousePos.x;
        cmd.y = OMousePos.y;
        Globals::pRTS->sendCommand(CMD_MOVE_AVATAR, &cmd);
    }
}

void Game::render()
{
    // Draw avatars
    OSB->begin();
    for (auto &user : Globals::myGame.users)
    {
        auto &avatar = avatarsByPlayerIds[user.id];
        OSB->drawSprite(OGetTexture("avatar.png"), avatar.pos);
    }
    OSB->end();
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

void Game::updateChats()
{
    for (auto pChatMsg : pChatContainer->getChildren())
    {
        auto pCharFader = (ChatFader*)pChatMsg->pUserData;
        pCharFader->update();
    }
}

ChatFader::ChatFader(onut::UILabel *pLabel)
    : m_pLabel(pLabel)
{
    pLabel->pUserData = this;
    m_alphaAnim.start(1.f,
    {
        {1.f, 5.f},
        {0.f, 2.f, onut::TweenType::LINEAR},
        {0.f, 0.f, onut::TweenType::LINEAR, [this]
        {
            m_pLabel->release();
            release();
        }}
    });
}

void ChatFader::update()
{
    m_pLabel->textComponent.font.color.a = m_alphaAnim.get();
}

void Game::onChatMessage(const std::string &msg)
{
    auto pNewChatMsg = dynamic_cast<onut::UILabel*>(pChatMsgTemplate->copy());
    for (auto pChatMsg : pChatContainer->getChildren())
    {
        pChatMsg->rect.position.y -= pNewChatMsg->rect.size.y;
    }
    pNewChatMsg->textComponent.text = msg;
    new ChatFader(pNewChatMsg);
    pChatContainer->add(pNewChatMsg);
}
