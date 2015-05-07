#pragma once
#include "onut.h"

class View
{
public:
    virtual ~View() {}

    virtual void enter() {}
    virtual void update() {}
    virtual void render() {}

    onut::UIControl *pUIScreen = nullptr;
};
