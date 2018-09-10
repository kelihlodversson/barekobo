#include "game/actor.h"
#include "game/world.h"
#include "game/partition.h"

using namespace hfh3;

Actor::Actor(World& inWorld)
    : world(inWorld)
    , stage(inWorld.GetStage())
{}
