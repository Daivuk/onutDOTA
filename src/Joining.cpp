#include "Joining.h"
#include "Main.h"

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
        delete g_pCurrentView;
        g_pCurrentView = new Main();
        g_pCurrentView->enter();
    };

//    addToRow(pUIScreen->getChild("topRow"), buildCardFromUser(Globals::myUser));

    // Send request to server to join to a game
    OAsync([this](Globals::SUser user)
    {
        auto ret = OStringFromURL("http://www.daivuk.com/onutDOTA/joinrequest.php",
        {{"userId", std::to_string(user.id)}, {"token", user.token}},
        [this](long status, const std::string &ret)
        {
            OSync([this, ret]
            {
                showError(ret);
            });
        });
        if (!ret.empty())
        {
            OSync([this, ret]
            {
                updateGameContent(ret);
            });
        }
    },
    Globals::myUser);

    lastUpdateRequestTime = std::chrono::steady_clock::now();
}

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
    // Parse json and validate
    rapidjson::Document doc;
    doc.Parse<0>(json.c_str());
    if (doc.IsNull())
    {
        showError("{\"message\":\"Error retreiving game info. code 1\"}");
        return;
    }
    const auto &jsonData = doc["data"];
    if (!jsonData.IsObject())
    {
        showError("{\"message\":\"Error retreiving game info. code 2\"}");
        return;
    }
    const auto &jsonGame = jsonData["game"];
    const auto &jsonUsers = jsonData["users"];
    if (!jsonGame.IsObject())
    {
        showError("{\"message\":\"Error retreiving game info. code 4\"}");
        return;
    }
    if (!jsonUsers.IsArray())
    {
        showError("{\"message\":\"Error retreiving game info. code 5\"}");
        return;
    }

    // Parse game
    Globals::myGame = {0};
    const auto &jsonGameId = jsonGame["id"];
    if (!jsonGameId.IsInt64())
    {
        showError("{\"message\":\"Error retreiving game info. code 6\"}");
        return;
    }
    Globals::myGame.id = jsonGameId.GetInt64();
    const auto &jsonGameSeed = jsonGame["seed"];
    if (!jsonGameSeed.IsUint())
    {
        showError("{\"message\":\"Error retreiving game info. code 7\"}");
        return;
    }
    Globals::myGame.seed = jsonGameSeed.GetUint();

    // Parse users
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
        delete g_pCurrentView;
        g_pCurrentView = new Main();
        g_pCurrentView->enter();
    };
}

void Joining::requestUpdate()
{
    if (!lastRequestCompleted) return;
    lastRequestCompleted = false;
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
                showError(ret);
            });
        });
        if (!ret.empty())
        {
            OSync([this, ret]
            {
                updateGameContent(ret);
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
