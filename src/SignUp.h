#pragma once
#include "View.h"

class SignUp : public View
{
public:
    SignUp();
    virtual ~SignUp();

    void enter() override;
    void update() override;
    void render() override;

    OAnimf windowAnimDown;
    bool isUsernameAvailable = false;
    bool usernameCheckAgain = false;
    bool usernameIsChecking = false;
    std::string usernameToCheck;
};
