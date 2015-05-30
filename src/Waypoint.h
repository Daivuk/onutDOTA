#pragma once

#include "Unit.h"

class Waypoint : public Unit
{
public:
    Waypoint();

    virtual void render();

    Unit *pPrevious[2]; // One for each team
    Unit *pNext[2]; // One for each team

    uint32_t nextWayPointId = 0;
};
