#include "Hero.h"
#include "Globals.h"
#include "FireShowerAbility.h"
#include "LightningBeamAbility.h"

Hero::Hero()
{
    abilities.push_back(new FireShowerAbility());
    abilities.push_back(new LightningBeamAbility());
    for (auto pAbility : abilities)
    {
        pAbility->pOwner = this;
    }
}

Hero::~Hero()
{
    for (auto pAbility : abilities)
    {
        delete pAbility;
    }
}

void Hero::onDestroyed()
{
    Globals::pMap->spawnFX(eFX::FX_ANIM_BLOOD_C, position);

    Globals::pMap->spawn(position, eUnitType::GIBS_HEAD, team, true);
    Globals::pMap->spawn(position, eUnitType::GIBS_LARM, team, true);
    Globals::pMap->spawn(position, eUnitType::GIBS_TORSO, team, true);
    Globals::pMap->spawn(position, eUnitType::GIBS_RARM, team, true);
    Globals::pMap->spawn(position, eUnitType::GIBS_LFOOT, team, true);
    Globals::pMap->spawn(position, eUnitType::GIBS_RFOOT, team, true);
}

void Hero::onPlayActionSound()
{
    static char filename[] = "hero1_action1.wav";
    filename[12] = '1' + (onut::randi() % 5);
    OGetSound(filename)->play(.5f);
}

void Hero::onUpdate()
{
    for (auto pAbility : abilities)
    {
        pAbility->rts_update();
    }
}

void Hero::drawUI()
{
    for (auto pAbility : abilities)
    {
        pAbility->render();
    }
}
