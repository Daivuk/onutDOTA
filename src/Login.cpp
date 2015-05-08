#include "Login.h"
#include "SignUp.h"

extern onut::UIContext *g_pUIContext;
extern View *g_pCurrentView;
void hookButtonSounds(onut::UIControl *pScreen);

Login::Login()
{
    pUIScreen = new onut::UIControl("../../assets/ui/signin.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnQuit")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        PostQuitMessage(0); 
    };
    pUIScreen->getChild("btnSignUp")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        delete g_pCurrentView;
        g_pCurrentView = new SignUp();
        g_pCurrentView->enter();
    };
    pUIScreen->getChild("btnSignIn")->onClick = [this](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        pUIScreen->getChild("signingin")->isVisible = true;
    };
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
