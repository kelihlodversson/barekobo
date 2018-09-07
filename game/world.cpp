#include "game/actor.h"
#include "game/world.h"

using namespace hfh3;

void World::Update()
{
    /* Update is invoked in reverse, so topmost objects are updated before lower ones */
    for(auto iter = rbegin(); iter != rend(); iter++)
    {
        iter->Update();
    }
}

void World::Draw()
{
    /* Rendering happens from bottom to top, the reverse order of Update */
    for(auto iter = begin(); iter != end(); iter++)
    {
        iter->Draw();
    }
}

void World::CollisionCheck(Actor* collider)
{
    const Rect<int> bounds = collider->GetBounds();
    auto collided = FindLast([collider, bounds](Actor* other) -> bool
    {
        return (other != collider) && (other->GetBounds() & bounds).IsValid();
    });

    if(collided)
    {
        collided->OnCollision(collider);
        collider->OnCollision(collided);
    }
}
