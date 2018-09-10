#include "game/actor.h"
#include "game/world.h"
#include "game/stage.h"

using namespace hfh3;

Actor::Actor(World& inWorld)
    : world(inWorld)
    , stage(inWorld.GetStage())
    , shouldDestruct(false)
    , positionDirty(true)
{}

void Actor::SetPosition(const Vector<int>& newPosition)
{
    position = stage.WrapCoordinate(newPosition);
    positionDirty = true;
}
