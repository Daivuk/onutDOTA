#pragma once
#include "Ability.h"
#include "Unit.h"
#include "Globals.h"
#include <vector>

class Hero : public Unit
{
public:
    Hero();
    virtual ~Hero();

    void drawUI();

    virtual void onDestroyed() override;
    virtual void onPlayActionSound() override;
    virtual void onUpdate() override;
    
    Globals::SUser *pUser = nullptr;

    std::vector<Ability *> abilities;
};
