#include <Windows.h>
#include "Joining.h"
#include "Main.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);
extern View *g_pCurrentView;

WSADATA wsa;

void Stun(const char *pAddr, uint16_t port)
{
    // Create a socket
    sockaddr_in toAddr = {0};
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(port);
    auto remoteHost = gethostbyname(pAddr);
    int i = 0;
    while (remoteHost->h_addr_list[i])
    {
        toAddr.sin_addr.S_un.S_addr = *(u_long *)remoteHost->h_addr_list[i++];

        SOCKET sock;
        if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
        {
            OutputDebugStringA("Failed to create socket\n");
            return;
        }

        if (sendto(sock, NULL, 0, 0, (struct sockaddr *)&toAddr, sizeof(toAddr)) == SOCKET_ERROR)
        {
            OutputDebugStringA("Stun failed sendto: ");
            OutputDebugStringA(pAddr);
            OutputDebugStringA("\n");
            continue;
        }

        int recv_len;
        int curBuf = 0;
        char pBuf[256];

        static int timeout = 1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        // Try to receive some data, this is a blocking call (But it's ok, we're in a thread yo)
        if ((recv_len = recvfrom(sock, pBuf, 256, 0, NULL, NULL)) == SOCKET_ERROR)
        {
            // We might just have killed the thread
            OutputDebugStringA("Stun failed recvfrom: ");
            OutputDebugStringA(pAddr);
            OutputDebugStringA("\n");
        }
        else
        {
            if (recv_len)
            {
                pBuf[255] = '\0';
                if (recv_len < 255) pBuf[recv_len] = '\0';
            }

            OutputDebugStringA("Stun response from ");
            OutputDebugStringA(pAddr);
            OutputDebugStringA(": ");
            OutputDebugStringA(pBuf);
            OutputDebugStringA("\n");
        }

        if (sock != INVALID_SOCKET)
        {
            closesocket(sock);
        }
        sock = INVALID_SOCKET;
    }
}

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
        g_pCurrentView->release();
        g_pCurrentView = new Main();
        g_pCurrentView->retain();
        g_pCurrentView->enter();
    };

    // Send request to server to join to a game
    retain();
    OAsync([this](Globals::SUser user)
    {
        auto ret = OStringFromURL("http://www.daivuk.com/onutDOTA/joinrequest.php",
        {{"userId", std::to_string(user.id)}, {"token", user.token}},
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

    // Initialise winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    Stun("www.daivuk.com", 9999);
}

/*
{url:'stun:stun01.sipphone.com'},
{url:'stun:stun.ekiga.net'},
{url:'stun:stun.fwdnet.net'},
{url:'stun:stun.ideasip.com'},
{url:'stun:stun.iptel.org'},
{url:'stun:stun.rixtelecom.se'},
{url:'stun:stun.schlund.de'},
{url:'stun:stun.l.google.com:19302'},
{url:'stun:stun1.l.google.com:19302'},
{url:'stun:stun2.l.google.com:19302'},
{url:'stun:stun3.l.google.com:19302'},
{url:'stun:stun4.l.google.com:19302'},
{url:'stun:stunserver.org'},
{url:'stun:stun.softjoys.com'},
{url:'stun:stun.voiparound.com'},
{url:'stun:stun.voipbuster.com'},
{url:'stun:stun.voipstunt.com'},
{url:'stun:stun.voxgratia.org'},
{url:'stun:stun.xten.com'},
*/

Joining::~Joining()
{
    g_pUIContext->clearState();
    pUIScreen->release();
    pUserCardTemplate->release();
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
        g_pCurrentView->release();
        g_pCurrentView = new Main();
        g_pCurrentView->retain();
        g_pCurrentView->enter();
    };
}

void Joining::requestUpdate()
{
    if (!lastRequestCompleted) return;
    lastRequestCompleted = false;
    retain();
    OAsync([this](Globals::SUser user, Globals::SGame game)
    {
        auto ret = OStringFromURL("http://www.daivuk.com/onutDOTA/gameinfo.php",
        {
            {"userId", std::to_string(user.id)}, 
            {"token", user.token},
            {"gameId", std::to_string(game.id)}
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
    pCard->getChild<onut::UILabel>("lblLevel")->textComponent.text = "Level " + std::to_string(user.level);
    return pCard;
}
