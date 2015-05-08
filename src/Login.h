#pragma once
#include "View.h"

class Login : public View
{
public:
    Login();
    virtual ~Login();

    void enter() override;
    void update() override;
    void render() override;

    OAnimf windowAnimDown;
};
