#pragma once
#include "View.h"

class Main : public View
{
public:
    Main();
    virtual ~Main();

    void enter() override;
    void update() override;
    void render() override;

    OAnimf windowAnimDown;
};

extern View *g_pCurrentView;
extern onut::UIContext *g_pUIContext;

template<typename Tview>
void changeView()
{
    g_pUIContext->clearState();
    auto *pNewView = new Tview();
    g_pCurrentView->release();
    g_pCurrentView = pNewView;
    g_pCurrentView->retain();
    g_pCurrentView->enter();
}
