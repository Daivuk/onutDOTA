#pragma once

#include "Unit.h"
#include "Globals.h"

class Hero : public Unit
{
public:
    Hero();

    virtual void onDestroyed() override;
    virtual void onPlayActionSound() override;

    Globals::SUser *pUser = nullptr;
};
