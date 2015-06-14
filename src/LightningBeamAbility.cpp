#include "LightningBeamAbility.h"
#include "Globals.h"

LightningBeamAbility::LightningBeamAbility()
{
}

void LightningBeamAbility::rts_update()
{
    Ability::rts_update();

    if (isInstance)
    {
        beamDuration -= ODT;
        if (beamDuration <= 0.f)
        {
            markForDeletion();
            return;
        }
        if (!pOwner || !pTarget)
        {
            markForDeletion();
            return;
        }
        beamAnim += ODT * 24.f;
        beamDamageDelay -= ODT;
        if (beamDamageDelay <= 0.f)
        {
            beamDamageDelay += 0.25f;
            if (pTarget)
            {
                Globals::pMap->spawnFX(eFX::FX_ANIM_LIGHTNING_SHOCK, pTarget->position);
                if (pTarget->damage(pType->damage))
                {
                    if (pOwner)
                    {
                        pOwner->kills++;
                    }
                }
                if (pTarget->pType->category == eUnitCategory::GROUND)
                {
                    Globals::pMap->spawnDecal(eFX((int)eFX::FX_DECAL_BLOOD_A + onut::randi(1)), pTarget->position, 0, .75f, 1.5f);
                }
            }
            if (pOwner)
            {
                Globals::pMap->spawnFX(eFX::FX_ANIM_LIGHTNING_SHOCK_BIG, pOwner->position);
            }
        }
    }
}

void LightningBeamAbility::render()
{
    Ability::render();

    if (isInstance)
    {
        if (pOwner && pTarget)
        {
            float beamFrame = (float)(int)beamAnim / 10.f;
            Vector2 dir = pTarget->position - pOwner->position;
            dir.Normalize();
            OSB->drawBeam(pType->pTexture, pOwner->position + dir * pOwner->pType->radius, pTarget->position, 1.5f, {1, 1, 1, 1}, beamFrame, 40.f / 1.5f);
        }
    }
}

void LightningBeamAbility::triggerAbility(Unit *in_pTarget)
{
    Ability::triggerAbility(in_pTarget);

    if (pOwner && in_pTarget)
    {
        auto pAbility = dynamic_cast<LightningBeamAbility*>(Globals::pMap->spawn(pOwner->position, eUnitType::ABILITY_LIGTHNING_BEAM, pOwner->team));
        pAbility->isInstance = true;
        pAbility->pOwner = pOwner;
        pAbility->pTarget = in_pTarget;
        pAbility->onSpawn();
    }
}

void LightningBeamAbility::onSpawn()
{
    OGetSound("lightningBeam.wav")->play();
}
