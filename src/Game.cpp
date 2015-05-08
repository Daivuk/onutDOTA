#include "Game.h"

extern onut::UIContext *g_pUIContext;
void hookButtonSounds(onut::UIControl *pScreen);

Game::Game()
{
    pUIScreen = new onut::UIControl("../../assets/ui/game.json");
    pUIScreen->retain();
    hookButtonSounds(pUIScreen);
}

Game::~Game()
{
    g_pUIContext->clearState();
    pUIScreen->release();
}

void Game::update()
{
}

void Game::render()
{
}
