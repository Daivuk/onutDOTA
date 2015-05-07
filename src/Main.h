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
