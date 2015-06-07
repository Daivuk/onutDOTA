#include "Minion.h"
#include "Waypoint.h"
#include "Globals.h"

Minion::Minion()
{
}

void Minion::setWayPoint(Unit *in_pWaypoint)
{
    pWaypoint = in_pWaypoint;
    if (pWaypoint)
    {
        attackTo(pWaypoint->getCenter());
    }
}

void Minion::onReachDestination()
{
    auto pWayPointWayPoint = dynamic_cast<Waypoint*>(pWaypoint);
    if (pWayPointWayPoint)
    {
        pWaypoint = pWayPointWayPoint->pNext[team];
        if (pWaypoint)
        {
            attackTo(pWaypoint->getCenter());
        }
    }
}

void Minion::onDestroyed()
{
    Globals::pMap->spawnFX(eFX::FX_ANIM_BLOOD_C, position);
}
