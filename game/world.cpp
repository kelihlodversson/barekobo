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
    , partitionSize(stage.GetSize() / partitionGridCount)
{
    // Initial partitioning: partition the world into 8x8+1 partitions:
    Rect<int> bounds ({0,0}, partitionSize);
    for(int y = 0; y < partitionGridCount; y++,  bounds.origin.y += partitionSize.y)
    {
        bounds.origin.x = 0;
        for(int x = 0; x < partitionGridCount; x++, bounds.origin.x += partitionSize.x)
        {
            GetPartition(x,y).SetBounds(bounds);
        }
    }
}

World::~World()
{
    for(Partition& partition : partitions)
    {
        for(auto iter = partition.begin(); iter != partition.end(); ++iter)
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
        for(Actor* actor : partition.Reverse())
        {
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
    PerformCollisionCheck();
    
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
    int x_min,x_max,y_min,y_max;
    GetPartitionRange(stage.GetVisibleRect(),x_min,x_max,y_min,y_max);
    for (int y = y_min; y < y_max; y++)
    {
        for (int x=x_min; x < x_max; x++)
        {
            for(Actor* actor : GetPartition(x,y))
            {
                actor->Draw();
            }

        }
    }
}

void World::PerformCollisionCheck()
{
    int x_min,x_max,y_min,y_max;
    List<Actor*>::ReverseIterator found;

    for(Actor* collider : collisionSources)
    {
        if(collider->collisionSourceMask != CollisionMask::None)
        {
            const Rect<int> bounds = collider->GetBounds();
            GetPartitionRange(bounds, x_min, x_max, y_min, y_max);
            for (int y = y_min; y < y_max; y++)
            {
                for (int x=x_min; x < x_max; x++)
                {
                    found = GetPartition(x,y).FindLast([collider, &bounds](Actor* other)
                    {
                        assert(other);
                        return collider->CollisionCheck(other);
                    });
                }
            }

            if(found)
            {
                Actor* collided = *found;
                collided->OnCollision(collider);
                collider->OnCollision(collided);
            }
        }
    }
}

void World::SpawnEnemy()
{
    AddActor(new Enemy(*this, imageSheet, random));
}

void World::SpawnPlayer()
{
    AddActor(new Player(*this, imageSheet, input));
}

void World::SpawnMissile(const Vector<int>& startPosition, const Direction& direction, int speed)
{
    AddActor(new Shot(*this, imageSheet, ImageSet::Missile, startPosition, direction, speed));
}

void World::AddActor(Actor* newActor)
{
    if(newActor->collisionSourceMask != CollisionMask::None)
    {
        collisionSources.Prepend(newActor);
    }
    needsNewPartition.Append(newActor);
}

void World::PerformPendingDeletes()
{
    for(Actor* actor : pendingDelete)
    {
        assert(actor->shouldDestruct);
        if(actor->collisionSourceMask != CollisionMask::None)
        {
            auto found = collisionSources.FindFirst([&actor](Actor* other){ return actor == other; });
            assert(found);
            found.Remove();
        }

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
        if (actor->partitionIterator)
        {
            assert(*actor->partitionIterator == actor);
            actor->partitionIterator.Remove();
        }

        // Add it to the new partition and save the returned iterator
        actor->partitionIterator = GetPartition(actor->position).Append(actor);
    }
    needsNewPartition.ClearFast();
}

void World::GetPartitionRange(const Rect<int>& rect, int& x1, int& x2, int& y1, int& y2)
{
    x1 = rect.Left() / partitionSize.x;
    x2 = 1 + rect.Right() / partitionSize.x;
    y1 = rect.Top() / partitionSize.y;
    y2 = 1 + rect.Bottom() / partitionSize.y;

    int remainder_x1 =  rect.Left() % partitionSize.x;
    int remainder_y1 =  rect.Top() % partitionSize.y;

    if (remainder_x1 < maxActorSize)
    {
        x1--;
    }
    if (remainder_y1 < maxActorSize)
    {
        y1--;
    }
}

void World::GameLoop()
{
    ScreenManager& screenManager = stage.GetScreen();

    Rect<int> clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    CString message;
    CString pos;
    CString tmp;

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

        int x_min,x_max,y,y_max;
        GetPartitionRange(stage.GetVisibleRect(),x_min,x_max,y,y_max);

        pos.Format("Offset: (%d,%d) (%d..%d),(%d..%d)",
            stage.GetOffset().x, stage.GetOffset().y,
            x_min, x_max,
            y, y_max);

        for(;y < y_max;y++) {for(int x = x_min;x < x_max;x++)
        {
            tmp.Format(" %d", (y & partitionGridMask)*partitionGridCount + (x & partitionGridMask));
            //tmp.Format(" %d,%d", (y & partitionGridMask), (x & partitionGridMask));
            pos.Append(tmp);
        }}
        screenManager.Clear(10);
        screenManager.DrawString({1,1}, message, 0, Font::GetDefault());
        screenManager.DrawString({1,clippedArea.Bottom()}, pos, 0, Font::GetDefault());
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);

        Update();
        Draw();

        screenManager.ClearClip();
        screenManager.Present();
    }

}
