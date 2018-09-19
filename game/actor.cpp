#include "game/actor.h"
#include "game/world.h"
#include "game/stage.h"

using namespace hfh3;

Actor::Actor(World& inWorld, CollisionMask inCollisionTargetMask, CollisionMask inCollisionSourceMask)
    : world(inWorld)
    , stage(inWorld.GetStage())
    , shouldDestruct(false)
    , positionDirty(true)
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
    return this != other && 
           (collisionSourceMask & other->collisionTargetMask) &&
           GetBounds().OverlapsMod(other->GetBounds(), stage.GetSize());
}