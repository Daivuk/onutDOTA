#pragma once

#include "Unit.h"

class Arrow : public Unit
{
public:
    virtual void onSpawn() override;
    virtual void render() override;
    virtual void rts_update() override;
};
