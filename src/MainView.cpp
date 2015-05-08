#include "Main.h"
#include "Game.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);
extern View *g_pCurrentView;

Main::Main()
{
    pUIScreen = new onut::UIControl("../../assets/ui/main.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnPlay")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        delete g_pCurrentView;
        g_pCurrentView = new Game();
        g_pCurrentView->enter();
    };
    pUIScreen->getChild("btnQuit")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        PostQuitMessage(0);
    };
}

Main::~Main()
{
    g_pUIContext->clearState();
    pUIScreen->release();
}

void Main::enter()
{
    windowAnimDown.start(-1000.f, 0.f, .5f, OEaseOut);
    OPlaySound("chainDrop.wav");
}

void Main::update()
{
    auto pWindow = pUIScreen->getChild("window");
    pWindow->rect.position.y = windowAnimDown.get();
}

void Main::render()
{
}
