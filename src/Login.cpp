#include "Login.h"
#include "SignUp.h"
#include "Globals.h"
#include "Main.h"

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
        g_pCurrentView->release();
        g_pCurrentView = new SignUp();
        g_pCurrentView->retain();
        g_pCurrentView->enter();
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
            auto ret = OStringFromURL("http://www.daivuk.com/onutDOTA/signin.php",
                {{"email", email}, {"password", password}},
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
                        g_pCurrentView->release();
                        g_pCurrentView = new Main();
                        g_pCurrentView->retain();
                        g_pCurrentView->enter();
                    }
                });
            }
        }, pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text, sha1);
    };

    pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text = OSettings->getUserSetting("email");
    pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text = OSettings->getUserSetting("password");
    refreshSignInEnableState(nullptr, {});
}

Login::~Login()
{
    g_pUIContext->clearState();
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
