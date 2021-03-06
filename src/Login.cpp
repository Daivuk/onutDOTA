#include "Login.h"
#include "SignUp.h"
#include "Globals.h"
#include "Main.h"
#include "Game.h"

extern onut::UIContext *g_pUIContext;
extern View *g_pCurrentView;
void hookButtonSounds(onut::UIControl *pScreen);

Login::Login()
{
    pUIScreen = new onut::UIControl("../../assets/ui/signin.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnSignIn")->isEnabled = false;
    auto refreshSignInEnableState = [this](onut::UITextBox* pTextBox, const onut::UITextBoxEvent&)
    {
        pUIScreen->getChild("btnSignIn")->isEnabled =
            !pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text.empty() &&
            !pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text.empty();
    };
    pUIScreen->getChild<onut::UITextBox>("txtEmail")->onTextChanged = refreshSignInEnableState;
    pUIScreen->getChild<onut::UITextBox>("txtPassword")->onTextChanged = refreshSignInEnableState;
    pUIScreen->getChild("btnQuit")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        PostQuitMessage(0); 
    };
    pUIScreen->getChild("btnSignUp")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<SignUp>();
    };
    pUIScreen->getChild("btnSignIn")->onClick = [this](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        pUIScreen->getChild("lblError")->isVisible = false;

        // Validate things locally first
        if (!onut::validateEmail(pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text.c_str()))
        {
            pUIScreen->getChild("lblError")->isVisible = true;
            pUIScreen->getChild<onut::UILabel>("lblError")->textComponent.text = "Email appears to be invalid";
            return;
        }

        // Hash password
        auto &pwd = pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text;
        auto sha1 = onut::sha1(pwd);

        // Sign in async
        pUIScreen->getChild("signingin")->isVisible = true;
        OAsync([this](std::string email, std::string password)
        {
            auto ret = OHTTPPost("http://www.daivuk.com/onutDOTA/signin.php",
            {{"email", email}, {"password", password}, {"version", std::to_string(VERSION)}},
            [this, email](long status, const std::string &ret)
            {
                OSync([this, ret, email]
                {
                    OSettings->setUserSetting("email", email);
                    OSettings->setUserSetting("password", "");
                    pUIScreen->getChild("signingin")->isVisible = false;
                    rapidjson::Document doc;
                    doc.Parse<0>(ret.c_str());
                    if (!doc.IsNull() && doc["message"].IsString())
                    {
                        pUIScreen->getChild("lblError")->isVisible = true;
                        pUIScreen->getChild<onut::UILabel>("lblError")->textComponent.text = doc["message"].GetString();
                    }
                });
            });
            if (!ret.empty())
            {
                OSync([this, ret, email, password]
                {
                    OSettings->setUserSetting("email", email);
                    OSettings->setUserSetting("password", pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text);
                    rapidjson::Document doc;
                    doc.Parse<0>(ret.c_str());
                    if (!doc.IsNull() && !doc["data"].IsNull() &&
                        doc["data"]["user"].IsObject())
                    {
                        Globals::SUser user;
                        Globals::userFromJson(user, doc["data"]["user"]);
                        Globals::setMyUser(user);
                        changeView<Main>();
                    }
                });
            }
        }, pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text, sha1);
    };
    pUIScreen->getChild("btnOfflinePlay")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        // Fake user
        Globals::myUser.connected = true;
        Globals::myUser.token = "";
        Globals::myUser.id = 1;
        Globals::myUser.username = "John Doe";
        Globals::myUser.level = 1;
        Globals::myUser.xp = 0;
        Globals::myUser.team = rand() % 2;
        Globals::myUser.ipPort = "127.0.0.1:9999";

        // Fake game
        Globals::myGame.id = 1;
        Globals::myGame.seed = 0;
        Globals::myGame.status = "IN_PROGRESS";
        Globals::myGame.users = {Globals::myUser};

        // RTS
        Globals::pRTS = new onut::RTS();
        Globals::pRTS->retain();
        Globals::pRTS->addMe(nullptr, Globals::myUser.id);

        changeView<Game>();

        Globals::pRTS->release();
    };

    pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text = OSettings->getUserSetting("email");
    pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text = OSettings->getUserSetting("password");
    refreshSignInEnableState(nullptr, {});
}

Login::~Login()
{
    pUIScreen->release();
}

void Login::enter()
{
    windowAnimDown.start(-1000.f, 0.f, .5f, OEaseOut);
    OPlaySound("chainDrop.wav");
}

void Login::update()
{
    auto pWindow = pUIScreen->getChild("window");
    pWindow->rect.position.y = windowAnimDown.get();
}

void Login::render()
{
}
