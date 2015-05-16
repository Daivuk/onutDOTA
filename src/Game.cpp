#include "Game.h"
#include "Globals.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Game::Game()
{
    pUIScreen = new onut::UIControl("../../assets/ui/game.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);

    Globals::pRTS->retain();
}

Game::~Game()
{
    pUIScreen->release();
    Globals::pRTS->release();
}

void Game::update()
{
    auto updateFrames = Globals::pRTS->update();
    while (updateFrames--)
    {
    }

    pUIScreen->getChild<onut::UILabel>("lblTurn")->textComponent.text = "Turn: " + std::to_string(Globals::pRTS->getTurn());
}

void Game::render()
{
}

void Game::enter()
{
    Globals::pRTS->start();
}
