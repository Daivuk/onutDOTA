#include "SignUp.h"
#include "Login.h"

extern onut::UIContext *g_pUIContext;
extern View *g_pCurrentView;
void hookButtonSounds(onut::UIControl *pScreen);

SignUp::SignUp()
{
    pUIScreen = new onut::UIControl("../../assets/ui/signup.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnCancel")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        delete g_pCurrentView;
        g_pCurrentView = new Login();
        g_pCurrentView->enter();
    };
}

SignUp::~SignUp()
{
    g_pUIContext->clearState();
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
}

void SignUp::render()
{
}
