#include <Windows.h>
#include "Joining.h"
#include "Main.h"
#include "Game.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);
extern View *g_pCurrentView;

Joining::Joining()
{
    pUIScreen = new onut::UIControl("../../assets/ui/joining.json");
    pUIScreen->retain();

    pUserCardTemplate = pUIScreen->getChild("userCard");
    pUserCardTemplate->retain();
    pUserCardTemplate->remove();

    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnCancel")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<Main>();
    };

    pUIScreen->getChild("btnForceStart")->onClick = [this](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        forceStartGame();
    };
    pUIScreen->getChild("btnForceStart")->isVisible = false;

    // Send request to server to join to a game
    retain();
    OAsync([this](Globals::SUser user)
    {
        auto ret = OHTTPPost("http://www.daivuk.com/onutDOTA/joinrequest.php",
        {{"userId", std::to_string(user.id)}, {"token", user.token}, {"version", std::to_string(VERSION)}},
        [this](long status, const std::string &ret)
        {
            OSync([this, ret]
            {
                if (getRefCount() > 1)
                {
                    showError(ret);
                }
                release();
            });
        });
        if (!ret.empty())
        {
            OSync([this, ret]
            {
                if (getRefCount() > 1)
                {
                    state = eState::WAITING;
                    updateGameContent(ret);
                }
                release();
            });
        }
    },
    Globals::myUser);

    lastUpdateRequestTime = std::chrono::steady_clock::now();

    Globals::pRTS = new onut::RTS();
    Globals::pRTS->retain();

    // Nat punch through
    ips = onut::getLocalIPS();
#if _DEBUG
    ips.push_back("127.0.0.1"); // To run multiple instances on the same machine
#endif
    natPunchThrough();
}

Joining::~Joining()
{
    pUIScreen->release();
    pUserCardTemplate->release();
    Globals::pRTS->release();
}

void Joining::natPunchThrough()
{
    retain();
    OAsync([this]
    {
        auto pMySocket = onut::natPunchThrough("www.daivuk.com:9999");
        OSync([this, pMySocket]
        {
            if (getRefCount() > 1)
            {
                if (!pMySocket)
                {
                    showError("Could not Nat Punch");
                }
                else
                {
                    std::string allIPs;
                    for (auto & ip: ips)
                    {
                        allIPs += ip + ":";
                    }
                    Globals::myUser.ipPort = allIPs + pMySocket->getIPPort();
                    Globals::pRTS->addMe(pMySocket, Globals::myUser.id);

                    // Send my ip port to the server so other players can sync up
                    sendIPPort();
                }
            }
            else if (pMySocket)
            {
                pMySocket->release();
            }
            release();
        });
    });
}

void Joining::sendIPPort()
{
    OHTTPPostAsync("http://www.daivuk.com/onutDOTA/myipport.php",
    {
        {"userId", std::to_string(Globals::myUser.id)},
        {"token", Globals::myUser.token},
        {"ipPort", Globals::myUser.ipPort},
        {"version", std::to_string(VERSION)}
    });
}

void Joining::update()
{
    auto now = std::chrono::steady_clock::now();
    if (now - lastUpdateRequestTime >= std::chrono::seconds(1))
    {
        lastUpdateRequestTime = now;
        if (state == eState::WAITING)
        {
            requestUpdate();
        }
    }
    Globals::pRTS->update();
}

void Joining::render()
{
}

void Joining::updateGameContent(const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse<0>(json.c_str());
    if (doc.IsNull())
    {
        showError("{\"message\":\"Error retreiving game info. code 1\"}");
        return;
    }
    const auto &jsonData = doc["data"];

    // Parse users
    if (!Globals::gameFromJson(Globals::myGame, jsonData))
    {
        showError("{\"message\":\"Error retreiving game info. code 4\"}");
        return;
    }

    updateRTSPeers();

    // Refresh rows
    auto pTopRow = pUIScreen->getChild("topRow");
    auto pBottomRow = pUIScreen->getChild("bottomRow");
    pTopRow->removeAll();
    pBottomRow->removeAll();
    for (const auto &user : Globals::myGame.users)
    {
        if (user.team == 0)
        {
            addToRow(pTopRow, buildCardFromUser(user));
        }
        else
        {
            addToRow(pBottomRow, buildCardFromUser(user));
        }
    }

    // Start the game if status has changed
    if (Globals::myGame.status == "IN_PROGRESS")
    {
        changeView<Game>();
    }

    if (!Globals::myUser.ipPort.empty())
    {
        pUIScreen->getChild("btnForceStart")->isVisible = true;
        for (auto pPeer : Globals::pRTS->getPeers())
        {
            if (!pPeer->isConnected())
            {
                pUIScreen->getChild("btnForceStart")->isVisible = false;
                break;
            }
        }
    }
}

void Joining::updateRTSPeers()
{
    // Remove gone peers
    auto peers = Globals::pRTS->getPeers();
    for (auto pPeer : peers)
    {
        bool bIsFound = false;
        for (auto &user : Globals::myGame.users)
        {
            if (pPeer->getPlayerId() == user.id)
            {
                bIsFound = true;
                break;
            }
        }
        if (!bIsFound)
        {
            Globals::pRTS->removePeer(pPeer);
        }
    }

    // Add new peers
    peers = Globals::pRTS->getPeers();
    for (auto &user : Globals::myGame.users)
    {
        if (user.id == Globals::myUser.id) continue;
        bool bIsFound = false;
        for (auto pPeer : peers)
        {
            if (pPeer->getPlayerId() == user.id)
            {
                bIsFound = true;
            }
        }
        if (!bIsFound)
        {
            if (Globals::pRTS->getSocket())
            {
                Globals::pRTS->addPeer(new onut::RTSPeer(Globals::pRTS->getSocket(), user.ipPort, user.id));
            }
        }
    }
}

void Joining::showError(const std::string &json)
{
    state = eState::STATE_ERROR;
    rapidjson::Document doc;
    doc.Parse<0>(json.c_str());
    std::string errorMsg = "Unknown Error. code 3";
    if (!doc.IsNull() && doc["message"].IsString())
    {
        errorMsg = doc["message"].GetString();
    }
    pUIScreen->getChild("error")->isVisible = true;
    pUIScreen->getChild<onut::UILabel>("lblError")->textComponent.text = errorMsg;
    pUIScreen->getChild("btnOK")->onClick = [this](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<Main>();
    };
}

void Joining::requestUpdate()
{
    if (!lastRequestCompleted) return;
    lastRequestCompleted = false;
    retain();
    OAsync([this](Globals::SUser user, Globals::SGame game)
    {
        auto ret = OHTTPPost("http://www.daivuk.com/onutDOTA/gameinfo.php",
        {
            {"userId", std::to_string(user.id)}, 
            {"token", user.token},
            {"gameId", std::to_string(game.id)},
            {"version", std::to_string(VERSION)}
        },
        [this](long status, const std::string &ret)
        {
            OSync([this, ret]
            {
                if (getRefCount() > 1)
                {
                    showError(ret);
                }
                release();
            });
        });
        if (!ret.empty())
        {
            OSync([this, ret]
            {
                if (getRefCount() > 1)
                {
                    updateGameContent(ret);
                    lastRequestCompleted = true;
                }
                release();
            });
        }
    }, Globals::myUser, Globals::myGame);
}

void Joining::addToRow(onut::UIControl *pRow, onut::UIControl *pControl)
{
    pRow->add(pControl);

    // Reajust positions
    auto cardCount = pRow->getChildren().size();
    auto spacing = 1.f / static_cast<float>(cardCount + 1);
    for (decltype(cardCount) i = 0; i < cardCount; ++i)
    {
        auto pCard = pRow->getChildren()[i];
        pCard->rect.position.x = static_cast<float>(i + 1) * spacing - .5f;
    }
}

onut::UIControl *Joining::buildCardFromUser(const Globals::SUser &user)
{
    auto pCard = pUserCardTemplate->copy();
    pCard->getChild<onut::UILabel>("lblUsername")->textComponent.text = user.username;
    pCard->getChild<onut::UILabel>("lblLevel")->textComponent.text ="Level " + std::to_string(user.level);
    return pCard;
}

void Joining::forceStartGame()
{
    pUIScreen->getChild("btnForceStart")->isVisible = false;
    pUIScreen->getChild("forceStartSpinner")->isVisible = true;

    // We signal the master server so players are made aware
    OHTTPPostAsync("http://www.daivuk.com/onutDOTA/startgame.php",
    {
        {"userId", std::to_string(Globals::myUser.id)},
        {"token", Globals::myUser.token},
        {"gameId", std::to_string(Globals::myGame.id)},
        {"version", std::to_string(VERSION)}
    });
}
