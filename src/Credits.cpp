#include "Credits.h"
#include "Main.h"
#include "Globals.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);
extern View *g_pCurrentView;

Credits::Credits()
{
    pUIScreen = new onut::UIControl("../../assets/ui/credits.json");
    pUIScreen->retain();
    pUIScreen->add(Globals::pUIHeader);
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnBack")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        g_pCurrentView->release();
        g_pCurrentView = new Main();
        g_pCurrentView->retain();
        g_pCurrentView->enter();
    };
}

Credits::~Credits()
{
    g_pUIContext->clearState();
    pUIScreen->release();
}

void Credits::enter()
{
    windowAnimDown.start(-1000.f, 0.f, .5f, OEaseOut);
    OPlaySound("chainDrop.wav");
}

void Credits::update()
{
    auto pWindow = pUIScreen->getChild("window");
    pWindow->rect.position.y = windowAnimDown.get();
}

void Credits::render()
{
}
