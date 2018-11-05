#include "game/gameserver.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"

#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"

#include "game/actor.h"
#include "game/base.h"
#include "game/enemy.h"
#include "game/player.h"
#include "game/shot.h"
#include "game/view.h"
#include "game/commandbuffer.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>

using namespace hfh3;

GameServer::GameServer(ScreenManager& inScreen, class Input& inInput, Network& inNetwork)
    : World(inScreen, inInput, inNetwork)
    , partitionSize(stage.GetSize() / partitionGridCount)
    , client(nullptr)
    , clientCommands(imageSheet)
{
    // Initial partitioning: partition the GameServer into 8x8 partitions:
    Rect<s16> bounds ({0,0}, partitionSize);
    for(int y = 0; y < partitionGridCount; y++,  bounds.origin.y += partitionSize.y)
    {
        bounds.origin.x = 0;
        for(int x = 0; x < partitionGridCount; x++, bounds.origin.x += partitionSize.x)
        {
            GetPartition(x,y).SetBounds(bounds);
        }
    }
}

GameServer::~GameServer()
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

    if(readerTask)
    {
        delete readerTask;
        readerTask = nullptr;
    }

    if(client)
    {
        delete client;
        client = nullptr;
    }
}

void GameServer::Update()
{
    for(Partition& partition : partitions)
    {
        Rect<s16> bounds = partition.GetBounds();
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
                    needsNewPartition.Append(actor);
                }
            }
        }
    }

    AssignPartitions();
    PerformPendingDeletes();
    PerformCollisionCheck();

    BuildCommandBuffer(player, commands);
    if(client)
    {
        BuildCommandBuffer(remotePlayer, clientCommands);
        clientCommands.Send(client);
    }
}

void GameServer::BuildCommandBuffer(Actor* player, CommandBuffer& commandBuffer)
{
    if(!player)
    {
        return;
    }

    Rect<s16> playerBounds = player->GetBounds();
    View view = View(stage, screen);
    view.SetCenterOffset(playerBounds.origin+playerBounds.size/2);

    commandBuffer.Clear();
    commandBuffer.SetViewOffset(view.GetOffset());
    commandBuffer.DrawBackground();

    // Loop trhough all partitions and call render on actors in partitions that
    // extend into the visible area.
    int x_min,x_max,y_min,y_max;
    GetPartitionRange(view.GetVisibleRect(),x_min,x_max,y_min,y_max);
    for (int y = y_min; y < y_max; y++)
    {
        for (int x=x_min; x < x_max; x++)
        {
            for(Actor* actor : GetPartition(x,y))
            {
                if(view.IsVisible(actor->GetBounds()))
                {
                    actor->Draw(commandBuffer);
                }
            }

        }
    }
}

void GameServer::PerformCollisionCheck()
{
    int x_min,x_max,y_min,y_max;
    List<Actor*>::ReverseIterator found;

    for(Actor* collider : collisionSources)
    {
        if(collider->collisionSourceMask != CollisionMask::None)
        {
            const Rect<s16> bounds = collider->GetBounds();
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

void GameServer::SpawnFortress()
{
    Rect<s16> area (20,0,20*16,20*16);
    Base::CreateFort(*this, imageSheet, random, area);
}

void GameServer::SpawnEnemy()
{
    AddActor(new Enemy(*this, imageSheet, random));
}

void GameServer::SpawnPlayer()
{
    player = new Player(*this, imageSheet, input);
    AddActor(player);
}

void GameServer::SpawnRemotePlayer()
{
    remotePlayer = new Player(*this, imageSheet, clientInput);
    remotePlayer->SetPosition({32,32});
    AddActor(remotePlayer);
}

void GameServer::SpawnMissile(const Vector<s16>& startPosition, const Direction& direction, int speed)
{
    AddActor(new Shot(*this, imageSheet, ImageSet::Missile, startPosition, direction, speed));
}

void GameServer::AddActor(Actor* newActor)
{
    if(newActor->collisionSourceMask != CollisionMask::None)
    {
        collisionSources.Prepend(newActor);
    }
    needsNewPartition.Append(newActor);
}

void GameServer::PerformPendingDeletes()
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

        if(actor == player)
        {
            player = nullptr;
        }

        if(actor->partitionIterator)
        {
            assert(*actor->partitionIterator == actor);
            actor->partitionIterator.Remove();
        }
        delete actor;
    }
    pendingDelete.Clear();
}

void GameServer::AssignPartitions()
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
    needsNewPartition.Clear();
}

void GameServer::GetPartitionRange(const Rect<s16>& rect, int& x1, int& x2, int& y1, int& y2)
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

void GameServer::Bind()
{
    client = network.WaitForClient();
    if(client)
    {
        client->Send("HI!", 3, 0);

        DEBUG("Waiting for greeting");
        u8 buffer[FRAME_BUFFER_SIZE];
        client->Receive(buffer, FRAME_BUFFER_SIZE, 0);
        // TODO Verify greeting

        SpawnRemotePlayer();
        readerTask = new NetworkReader(client, clientInput); 
    }
}

void GameServer::NetworkReader::Run()
{
    CScheduler* scheduler = CScheduler::Get();
    while(true)
    {
        u8 buffer[FRAME_BUFFER_SIZE];
        int count = connection->Receive(buffer, FRAME_BUFFER_SIZE, MSG_DONTWAIT);
    
        if(count)
        {
            for(int i = 0; i < count; i++)
            {
                remoteInput.SetInputState(buffer[i]);
            }
        }
        else
        {
            // When there is no data ready, sleep for 60th of a second
            // before trying again. This will also yield control to other threads
            scheduler->MsSleep(16); 
        }
    }  
}