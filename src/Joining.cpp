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

    addToRow(pUIScreen->getChild("topRow"), buildCardFromUser(Globals::myUser));

    // Send request to server to join to a game
    OAsync([this](Globals::SUser user)
    {
        auto ret = OStringFromURL("http://www.daivuk.com/onutDOTA/joinrequest.php",
        {{"userId", std::to_string(user.id)}, {"token", user.token}},
        [this](long status, const std::string &ret)
        {
            int tmp;
            tmp = 5;
        });
        if (!ret.empty())
        {
            int tmp;
            tmp = 5;
        }
    },
    Globals::myUser);
}

Joining::~Joining()
{
    g_pUIContext->clearState();
    pUIScreen->release();
    pUserCardTemplate->release();
}

void Joining::update()
{
}

void Joining::render()
{
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

void Joining::removeFromRow(onut::UIControl *pRow, onut::UIControl *pControl)
{
    pRow->remove(pControl);

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
