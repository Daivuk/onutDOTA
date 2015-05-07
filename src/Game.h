#pragma once
#include "View.h"

class Game : public View
{
public:
    Game();
    virtual ~Game();

    void update() override;
    void render() override;
};
