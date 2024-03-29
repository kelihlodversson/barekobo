#include "game/actor.h"
#include "game/gameserver.h"
#include "game/stage.h"

using namespace hfh3;

Actor::Actor(GameServer& inWorld, CollisionMask inCollisionTargetMask, CollisionMask inCollisionSourceMask)
    : world(inWorld)
    , stage(inWorld.GetStage())
    , shouldDestruct(false)
    , positionDirty(true)
    , killer(-1)
    , collisionTargetMask(inCollisionTargetMask)
    , collisionSourceMask(inCollisionSourceMask)
{}

void Actor::SetPosition(const Vector<s16>& newPosition)
{
    position = stage.WrapCoordinate(newPosition);
    positionDirty = true;
}

bool Actor::CollisionCheck(class Actor* other)
{
    return this != other && !shouldDestruct && 
           (collisionSourceMask & other->collisionTargetMask) &&
           GetBounds().OverlapsMod(other->GetBounds(), stage.GetSize());
}

void Actor::Destroy()
{
    if(destructionHandler)
    {
        destructionHandler();
    }
    shouldDestruct = true;
}
