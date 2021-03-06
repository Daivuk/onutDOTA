#include "SignUp.h"
#include "Login.h"
#include "rapidjson/document.h"
#include "Globals.h"
#include "Main.h"

extern onut::UIContext *g_pUIContext;
extern View *g_pCurrentView;
void hookButtonSounds(onut::UIControl *pScreen);

SignUp::SignUp()
{
    pUIScreen = new onut::UIControl("../../assets/ui/signup.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnSignUp")->isEnabled = false;
    auto refreshSignUpEnableState = [this](onut::UITextBox* pTextBox, const onut::UITextBoxEvent&)
    {
        pUIScreen->getChild("btnSignUp")->isEnabled =
            !pUIScreen->getChild<onut::UITextBox>("txtUsername")->textComponent.text.empty() &&
            !pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text.empty() &&
            !pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text.empty() &&
            !pUIScreen->getChild<onut::UITextBox>("txtPasswordRepeat")->textComponent.text.empty() &&
            pUIScreen->getChild<onut::UITextBox>("txtPassword")->textComponent.text ==
            pUIScreen->getChild<onut::UITextBox>("txtPasswordRepeat")->textComponent.text &&
            isUsernameAvailable;
    };
    pUIScreen->getChild<onut::UITextBox>("txtUsername")->onTextChanged = 
        [this, refreshSignUpEnableState](onut::UITextBox* pTextBox, const onut::UITextBoxEvent& evt)
    {
        refreshSignUpEnableState(pTextBox, evt);
        // Send a validate username request
        if (pTextBox->textComponent.text.empty())
        {
            // Nothing to send
            pUIScreen->getChild("available")->isVisible = false;
            pUIScreen->getChild("taken")->isVisible = false;
        }
        else
        {
            usernameCheckAgain = true;
            usernameToCheck = pTextBox->textComponent.text;
        }
    };
    pUIScreen->getChild<onut::UITextBox>("txtEmail")->onTextChanged = refreshSignUpEnableState;
    pUIScreen->getChild<onut::UITextBox>("txtPassword")->onTextChanged = refreshSignUpEnableState;
    pUIScreen->getChild<onut::UITextBox>("txtPasswordRepeat")->onTextChanged = refreshSignUpEnableState;
    pUIScreen->getChild("btnCancel")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<Login>();
    };
    pUIScreen->getChild("btnSignUp")->onClick = [this](onut::UIControl*, const onut::UIMouseEvent&)
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

        // Sign up async
        pUIScreen->getChild("signingup")->isVisible = true;
        retain();
        OAsync([this](std::string username, std::string email, std::string password)
        {
            auto ret = OHTTPPost("http://www.daivuk.com/onutDOTA/signup.php",
            {{"username", username}, {"email", email}, {"password", password}, {"version", std::to_string(VERSION)}},
                                      [this, email](long status, const std::string &ret)
            {
                OSync([this, ret, email]
                {
                    if (getRefCount() > 1)
                    {
                        OSettings->setUserSetting("email", email);
                        OSettings->setUserSetting("password", "");
                        pUIScreen->getChild("signingup")->isVisible = false;
                        rapidjson::Document doc;
                        doc.Parse<0>(ret.c_str());
                        if (!doc.IsNull() && doc["message"].IsString())
                        {
                            pUIScreen->getChild("lblError")->isVisible = true;
                            pUIScreen->getChild<onut::UILabel>("lblError")->textComponent.text = doc["message"].GetString();
                        }
                    }
                    release();
                });
            });
            if (!ret.empty())
            {
                OSync([this, ret, email, password]
                {
                    if (getRefCount() > 1)
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
                    }
                    release();
                });
            }
        }, pUIScreen->getChild<onut::UITextBox>("txtUsername")->textComponent.text,
           pUIScreen->getChild<onut::UITextBox>("txtEmail")->textComponent.text,
           sha1);
    };
}

SignUp::~SignUp()
{
    pUIScreen->release();
}

void SignUp::enter()
{
    windowAnimDown.start(-1000.f, 0.f, .5f, OEaseOut);
    OPlaySound("chainDrop.wav");
}

void SignUp::update()
{
    auto pWindow = pUIScreen->getChild("window");
    pWindow->rect.position.y = windowAnimDown.get();

    // Username check
    if (!usernameIsChecking && usernameCheckAgain)
    {
        usernameCheckAgain = false;
        usernameIsChecking = true;
        pUIScreen->getChild("available")->isVisible = false;
        pUIScreen->getChild("taken")->isVisible = false;
        pUIScreen->getChild("spinner")->isVisible = true;
        retain();
        OAsync([this](std::string username)
        {
            auto ret = OHTTPPost("http://www.daivuk.com/onutDOTA/checkusrname.php",
            {{"username", username}, {"version", std::to_string(VERSION)}},
                                      [this](long status, const std::string &ret)
            {
                OSync([this, ret]
                {
                    if (getRefCount() > 1)
                    {
                        rapidjson::Document doc;
                        doc.Parse<0>(ret.c_str());
                        if (!doc.IsNull() && !doc["message"].IsString())
                        {
                            pUIScreen->getChild("lblError")->isVisible = true;
                            pUIScreen->getChild<onut::UILabel>("lblError")->textComponent.text = doc["message"].GetString();
                        }
                    }
                    release();
                });
            });
            OSync([this, ret]
            {
                if (getRefCount() > 1)
                {
                    pUIScreen->getChild("spinner")->isVisible = false;
                    pUIScreen->getChild("available")->isVisible = false;
                    pUIScreen->getChild("taken")->isVisible = false;
                    usernameIsChecking = false;

                    rapidjson::Document doc;
                    doc.Parse<0>(ret.c_str());
                    if (!doc.IsNull() && !doc["data"].IsNull() && doc["data"]["available"].IsBool())
                    {
                        isUsernameAvailable = doc["data"]["available"].GetBool();
                        if (isUsernameAvailable)
                        {
                            pUIScreen->getChild("available")->isVisible = true;
                            pUIScreen->getChild("taken")->isVisible = false;
                        }
                        else
                        {
                            pUIScreen->getChild("available")->isVisible = false;
                            pUIScreen->getChild("taken")->isVisible = true;
                        }
                    }
                }
                release();
            });
        }, usernameToCheck);
    }
}

void SignUp::render()
{
}
