#include "Game.h"
#include "Globals.h"
#include "commands.h"
#include "Minion.h"
#include "Spawner.h"

#define WAVE_INTERVAL 360000
#define WAVE_MINION_INTERVAL 90
#define WAVE_COUNT 5

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Game::Game()
{
    // Randomize
    srand(Globals::myGame.seed);

    Globals::rts_frame = 0;
    Minion::s_radiusCheckId = 0;

    Globals::pMap = new Map(0);
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
    Globals::pMap->release();
}

void Game::update()
{
    Globals::pMap->update();

    auto updateFrames = Globals::pRTS->update();
    while (updateFrames--)
    {
        rts_update();
    }

    updateChats();

    // Show current turn in debug
#if _DEBUG
    pUIScreen->getChild<onut::UILabel>("lblTurn")->textComponent.text = "Turn: " + std::to_string(Globals::pRTS->getTurn());
#endif
}

void Game::rts_update()
{
    Globals::rts_frame++;
    --nextWaveIn;
    if (nextWaveIn <= 0)
    {
        // Spawn minions from spawners!
        spawnMinions();
        minionSpawned++;
        if (minionSpawned == WAVE_COUNT)
        {
            minionSpawned = 0;
            nextWaveIn = WAVE_INTERVAL;
        }
        else
        {
            nextWaveIn = WAVE_MINION_INTERVAL;
        }
    }
    Globals::pMap->rts_update();
}

void Game::spawnMinions()
{
    std::vector<Spawner*> spawners = Globals::pMap->getUnits<Spawner>();
    for (auto pSpawner : spawners)
    {
        auto pMinion = dynamic_cast<Minion*>(Globals::pMap->spawn(pSpawner->getCenter(), eUnitType::MINION, pSpawner->team));
        pMinion->walkTo(pSpawner->pFirstWaypoint);
    }
}

void Game::render()
{
    Globals::pMap->render();
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
