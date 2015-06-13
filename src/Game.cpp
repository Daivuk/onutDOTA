#include "Game.h"
#include "Globals.h"
#include "commands.h"
#include "Minion.h"
#include "Spawner.h"
#include "SpawnPoint.h"
#include "Hero.h"

#define WAVE_INTERVAL 3600
#define WAVE_MINION_INTERVAL 45
#define WAVE_COUNT 10

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Game::Game()
    : onut::EventObserver(OEvent)
{
    // Randomize
    srand(Globals::myGame.seed);

    Globals::rts_frame = 0;

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

    // Spawn Heroes
    decltype(Globals::pMap->getUnits<SpawnPoint>(TEAM_RED)) spawns[2] = 
    {
        Globals::pMap->getUnits<SpawnPoint>(TEAM_RED), 
        Globals::pMap->getUnits<SpawnPoint>(TEAM_BLUE)
    };
    for (auto &user : Globals::myGame.users)
    {
        auto &teamSpawns = spawns[user.team];
        if (teamSpawns.empty()) continue; // Oups!
        auto spawnIndex = onut::randi() % teamSpawns.size();
        auto pSpawn = teamSpawns[spawnIndex];
        teamSpawns.erase(teamSpawns.begin() + spawnIndex);

        auto pHero = dynamic_cast<Hero*>(Globals::pMap->spawn(pSpawn->getCenter(), eUnitType::HERO, user.team));
        if (pHero)
        {
            pHero->pUser = &user;
            user.pUnit = pHero;

            if (user.id == Globals::myUser.id)
            {
                Globals::myUser.pUnit = pHero;
            }
        }
    }

    // Now focus the camera on our hero
    auto pMyHero = Globals::myUser.pUnit;
    if (pMyHero)
    {
        Globals::pMap->m_cameraPos = pMyHero->getCenter();
    }

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
    pUIScreen->onRightMouseDown = [=](onut::UIControl*, const onut::UIMouseEvent& evt)
    {
        auto pMyHero = Globals::myUser.pUnit;
        if (pMyHero)
        {
            if (OInput->isStateDown(DIK_LSHIFT))
            {
                pMyHero->playActionSound();
                sCMD_ATTACK_TO cmd;
                cmd.mapPos = Globals::pMap->screenToMap(onut::UI2Onut(evt.mousePos));
                Globals::pRTS->sendCommand(CMD_ATTACK_TO, &cmd);
                Globals::pMap->spawnFX(eFX::FX_ANIM_ATTACK_TO, cmd.mapPos);
            }
            else
            {
                pMyHero->playActionSound();
                sCMD_MOVE_TO cmd;
                cmd.mapPos = Globals::pMap->screenToMap(onut::UI2Onut(evt.mousePos));
                Globals::pRTS->sendCommand(CMD_MOVE_TO, &cmd);
                Globals::pMap->spawnFX(eFX::FX_ANIM_MOVE_TO, cmd.mapPos);
            }
        }
    };
    pUIScreen->getChild<onut::UIButton>("btnAbility")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OEvent->fireEvent("Trigger Ability 1");
    };
    OEvent->addEvent("Trigger Ability 1", []{return OInput->isStateJustDown(DIK_Q); });
    observe("Trigger Ability 1", []
    {
        auto pMyHero = dynamic_cast<Hero*>(Globals::myUser.pUnit);
        if (pMyHero)
        {
            if (pMyHero->abilities.size() >= 1)
            {
                for (auto pAbility : pMyHero->abilities)
                {
                    pAbility->cancel();
                }
                if (pMyHero->abilities[0]->canUse())
                {
                    pMyHero->abilities[0]->activate();
                }
            }
        }
    });
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
#if _DEBUG
    if (OInput->isStateDown(DIK_F))
    {
        updateFrames *= 2;
    }
#endif
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
        pMinion->setWayPoint(pSpawner->pFirstWaypoint);
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
