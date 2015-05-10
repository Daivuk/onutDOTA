#pragma once
#include "View.h"

class Credits : public View
{
public:
    Credits();
    virtual ~Credits();

    void enter() override;
    void update() override;
    void render() override;

    OAnimf windowAnimDown;
};
