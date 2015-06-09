#pragma once

#include "UnitAnim.h"

#define TEAM_NONE -1
#define TEAM_RED 0
#define TEAM_BLUE 1
#define TEAM_NEUTRAL 2

enum class eUnitType : uint8_t
{
    NONE,
    SPAWNER,
    NEXUS,
    WAYPOINT,
    MINION,
    ARROW,
    TOWER,
    GIBS_HEAD,
    GIBS_LARM,
    GIBS_TORSO,
    GIBS_RARM,
    GIBS_LFOOT,
    GIBS_RFOOT,
    HERO,
    SPAWN_POINT
};

enum class eUnitSizeType : uint8_t
{
    NONE,
    RADIUS,
    BOX
};

enum class eUnitCategory : uint8_t
{
    NONE,
    BUILDLING,
    GROUND,
    AIR,
    PROJECTILE
};

// The main state the unit is in. The player is aware of those states.
enum class eUnitState : uint8_t
{
    NONE,
    IDLE,
    HOLD,
    MOVE,
    FOLLOW,
    ATTACK_UNIT,
    ATTACK_POSITION,
    DEAD
};

enum class eUnitAttackType : uint8_t
{
    NONE,
    DIRECT,
    SPLASH_DAMAGE,
    PROJECTILE
};

struct sMapChunk;
class Unit;

class IUnitFactory
{
public:
    virtual Unit *create(OPool *pPool) = 0;
};

template<typename Tunit>
class UnitFactory : public IUnitFactory
{
public:
    Unit *create(OPool *pPool) override { return pPool->alloc<Tunit>(); }
};

struct sUnitType
{
    // Type info
    std::string     typeName;
    std::string     screenName;
    eUnitCategory   category = eUnitCategory::NONE;

    // Size info
    eUnitSizeType   sizeType = eUnitSizeType::NONE;
    POINT           boxSize;
    float           radius = 0;

    // Sprite / Anim info
    float           yOffset = 0;

    // Stats
    int             health = 0;
    int             armor = 0;
    int             mana = 0;

    // Vision
    float           visionRange = 0;
    float           alertRange = 0;

    // Movement
    float           moveSpeed = 0;

    // Attack
    eUnitAttackType attackType = eUnitAttackType::NONE;
    eUnitType       projectileUnitType = eUnitType::NONE;
    float           damage = 0;
    float           damageRadius = 0;
    float           attackRange = 0;
    float           attackDelay = 0;
    float           attackCoolDown = 0;

    // Anims
    std::unordered_map<int, UnitAnimDef*> anims;
    OTexture*       pTexture = nullptr;
    Vector4         UVs = {0, 0, 1, 1};

    // Factory
    IUnitFactory   *pFactory = nullptr;
};

class Unit
{
public:
    Unit();

    Vector2         getCenter() const;
    void            calculatePathToPos(const Vector2 &in_targetPos);
    void            progressPath();
    void            doMovement();
    void            steer(const Vector2 &otherPos, Vector2 &dir, float strength);
    void            steerFromTile(int tileX, int tileY, Vector2 &dir);
    bool            targetEscaped();
    bool            targetInAttackRange();
    Unit*           aquireTarget();
    void            performAttack();
    void            markForDeletion();
    bool            damage(float damage);

    // Updates
    virtual void    rts_update();
    void            rts_updateState();

    // Drawing
    virtual void    render();
#if _DEBUG
    virtual void    renderDebug();
#endif

    // State changes
    void            goIdle();
    void            attackTo(const Vector2 &attackPos);
    void            moveTo(const Vector2 &movePos);

    // Events
    virtual void onSpawn() {}
    virtual void onReachDestination() {}
    virtual void onTargetDestroyed(Unit *in_pTarget) {}
    virtual void onOnwerDestroyed(Unit *in_pOwner) {}
    virtual void onDestroyed() {}

    // Link to lists
    LIST_LINK(Unit) linkMain;
    LIST_LINK(Unit) linkChunk;

    // Type info
    eUnitType       type = eUnitType::NONE;
    sUnitType      *pType = nullptr;

    // Positionnal/size info
    Vector2         position;
    POINT           boxSize;
    sMapChunk      *pChunk = nullptr;

    // State
    eUnitState      state = eUnitState::IDLE;
    Unit           *pTarget = nullptr;
    Vector2         targetPos;
    Vector2         prevTargetPos;
    Vector2         missionTargetPos;
    int             team = TEAM_NONE;
    sUnitAnim       anim;
    int             direction = BALT_DOWN;
    int             animState = BALT_IDLE;
    Vector2         movingDirection;
    bool            animDirty = false;
    std::vector<Vector2> path;
    float           attackCooldown = 0;
    float           attackDelay = 0;
    Unit           *pOwner = nullptr;
    bool            bDeletionRequested = false;

    // Unit stats
    int             health = 100;
    int             armor = 0;
    int             mana = 0;

    // Unique map id generated by the map editor. Otherwise 0 if generated from game
    uint32_t        mapId = 0;

    int             kills = 0;
};
