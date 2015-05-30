#pragma once

#include "Unit.h"

class Waypoint;

class Spawner : public Unit
{
public:
    Unit *pFirstWaypoint = nullptr;
    uint32_t firstWaypoiuntId = 0;
};
