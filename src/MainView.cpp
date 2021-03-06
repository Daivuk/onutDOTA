#include "Main.h"
#include "Joining.h"
#include "Globals.h"
#include "Credits.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Main::Main()
{
    pUIScreen = new onut::UIControl("../../assets/ui/main.json");
    pUIScreen->retain();
    pUIScreen->add(Globals::pUIHeader);
    hookButtonSounds(pUIScreen);

    pUIScreen->getChild("btnPlay")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<Joining>();
    };
    pUIScreen->getChild("btnQuit")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        PostQuitMessage(0);
    };
    pUIScreen->getChild("btnCredits")->onClick = [](onut::UIControl*, const onut::UIMouseEvent&)
    {
        OPlaySound("buttonClick.wav");
        changeView<Credits>();
    };
}

Main::~Main()
{
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
