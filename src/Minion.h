#pragma once

#include "Unit.h"

class Minion : public Unit
{
public:
    Minion();

    virtual void onReachDestination() override;
    
    void setWayPoint(Unit *in_pWaypoint);

    Unit *pWaypoint = nullptr;
};
