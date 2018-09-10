#include "game/world.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"

#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"

#include "game/actor.h"
#include "game/enemy.h"
#include "game/player.h"
#include "game/shot.h"

using namespace hfh3;

World::World(ScreenManager& inScreen, class Input& inInput, Network& inNetwork)
    : stage(4096, 4096, inScreen)
    , input(inInput)
    , network(inNetwork)
    , imageSheet(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8)
    , background(*this)
{
    // Initial partitioning: partition the world into 8x8+1 partitions:
    const Vector<int> stageSize = stage.GetSize();
    const Vector<int> partitionSize = stageSize /8;

    Rect<int> bounds ({0,0}, partitionSize);
    for(bounds.origin.y = 0; bounds.origin.y < stageSize.y; bounds.origin.y += partitionSize.y)
    {
        for(bounds.origin.x = 0; bounds.origin.x < stageSize.x; bounds.origin.x += partitionSize.x)
        {
            partitions.Append(*this, bounds);
        }
    }
    assert(partitions.Size() == 8*8);
}

World::~World()
{
    for(auto partitionIter = partitions.begin(); partitionIter != partitions.end(); ++partitionIter)
    {
        for(auto iter = partitionIter->begin(); iter != partitionIter->end(); ++iter)
        {
            Actor* actor = *iter;
            if(actor == nullptr)
            {
                continue;
            }
            *iter = nullptr;

            delete actor;
        }
    }
}

void World::Update()
{
    for(Partition& partition : partitions)
    {
        Rect<int> bounds = partition.GetBounds();
        for(auto iter = partition.rbegin(); iter != partition.rend(); ++iter)
        {
            Actor* actor = *iter;
            assert(actor);
            actor->Update();

            if( actor->shouldDestruct)
            {
                pendingDelete.Append(actor);
            }
            // Check if item has moved out of the partition
            else if(actor->positionDirty)
            {
                actor->positionDirty = false;
                if(!bounds.Contains(actor->position) )
                {
                    needsNewPartition.Prepend(actor);
                }
            }
        }
    }

    AssignPartitions();
    PerformPendingDeletes();

    // The background object is special and is not stored in a partition
    background.Update();
}

void World::Draw()
{
    // The background object is special and is not stored in a partition
    // it should always be rendered first.
    background.Draw();

    // Loop trhough all partitions and call render on actors in partitions that
    // extend into the visible area.
    for(Partition& partition : partitions)
    {
        if(stage.IsVisible(partition.GetExtendedBounds()))
        {
            for(Actor* actor : partition)
            {
                actor->Draw();
            }
        }
    }
}

Actor* World::CollisionCheck(Actor* collider)
{
    const Rect<int> bounds = collider->GetBounds();

    List<Actor*>::ReverseIterator found;
    for(Partition& partition : partitions)
    {
        if( partition.GetExtendedBounds().Overlaps(bounds) )
        {
             found = partition.FindLast([collider, &bounds](Actor** other) -> bool
             {
                 assert(other);
                 assert(*other);
                 return (*other != collider) && (*other)->GetBounds().Overlaps(bounds);
             });
        }
    }

    if(found)
    {
        Actor* collided = *found;
        collided->OnCollision(collider);
        collider->OnCollision(collided);
        return collided;
    }
    else
    {
        return nullptr;
    }
}

void World::SpawnEnemy()
{
    needsNewPartition.Append(new Enemy(*this, imageSheet, random));
}

void World::SpawnPlayer()
{
    needsNewPartition.Append(new Player(*this, imageSheet, input));
}

void World::SpawnMissile(const Vector<int>& startPosition, const Direction& direction, int speed)
{
    needsNewPartition.Append(new Shot(*this, imageSheet, ImageSet::Missile, startPosition, direction, speed));
}

void World::PerformPendingDeletes()
{
    for(Actor* actor : pendingDelete)
    {
        assert(actor->shouldDestruct);
        if(actor->partitionIterator)
        {
            assert(*actor->partitionIterator == actor);
            actor->partitionIterator.Remove();
        }
        delete actor;
    }
    pendingDelete.ClearFast();
}

void World::AssignPartitions()
{
    for(Actor* actor : needsNewPartition)
    {
        const Vector<int> position = actor->GetBounds().origin;
        auto found = partitions.FindFirst([&position](Partition* partition)
        {
            return partition->GetBounds().Contains(position);
        });

        assert(found);

        // If the actor was already assigned to a partition, remove it from it
        if (actor->partitionIterator)
        {
            assert(*actor->partitionIterator == actor);
            actor->partitionIterator.Remove();
        }

        // Add it to the new partition and save the returned iterator
        actor->partitionIterator = found->Append(actor);
    }
    needsNewPartition.ClearFast();
}


void World::GameLoop()
{
    ScreenManager& screenManager = stage.GetScreen();

    Rect<int> clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    CString message;
    CString pos;

    while(true)
    {
        u32 ip = network.GetIPAddress();
        message.Format("IP: %u.%u.%u.%u. FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
            (ip & 0xff),
            (ip & 0xff00)>>8,
            (ip & 0xff0000)>>16,
            (ip & 0xff000000)>>24,
            screenManager.GetFPS(),
            screenManager.GetMissedFrames(),
            screenManager.GetGameTimePCT(),
            screenManager.GetFlipTimePCT()
        );

        Update();

        pos.Format("Offset: (%d,%d)", stage.GetOffset().x, stage.GetOffset().y);
        screenManager.Clear(10);
        screenManager.DrawString({1,1}, message, 0, Font::GetDefault());
        screenManager.DrawString({1,clippedArea.Bottom()}, pos, 0, Font::GetDefault());
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);

        Draw();

        screenManager.ClearClip();
        screenManager.Present();
    }

}
