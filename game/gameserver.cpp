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
#include "game/explosion.h"
#include "game/player.h"
#include "game/shot.h"
#include "game/view.h"
#include "game/commandbuffer.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>

using namespace hfh3;

GameServer::GameServer(MainLoop& inMainLoop, class Input& inInput, Network& inNetwork)
    : World(inMainLoop, inInput, inNetwork)
    , partitionSize(stage.GetSize() / partitionGridCount)
    , player({nullptr, stage.GetSize()/2})
    , remotePlayer({nullptr, stage.GetSize()/2})
    , baseCount(0)
    , client(nullptr)
    , clientCommands(imageSheet)
    , currentLevel(-1)
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

#ifdef DEBUG_GAMESERVER
void GameServer::Render()
{
    World::Render();
    const Rect<s16> player.actorBounds = player.actor->GetBounds();
    View view = View(stage, screen);
    view.SetCenterOffset(player.actorBounds.origin+player.actorBounds.size/2);

    int x_min,x_max,y_min,y_max;

    GetPartitionRange(player.actorBounds, x_min, x_max, y_min, y_max);
    CString d;

    d.Format("x: %d->%d y: %d->%d", x_min, x_max, y_min, y_max);
    screen.DrawString({20,20}, d, 22, Font::GetDefault());

    for (int y = y_min; y < y_max; y++)
    {
        int dy = y-y_min;
        for (int x=x_min; x < x_max; x++)
        {
            int dx = x-x_min;
            Partition& part = GetPartition(x,y);
            view.DrawRect(part.GetBounds(), dy*3 + dx+1);
        }
    }
    view.DrawRect(player.actorBounds, 20);
    for (int y = y_min; y < y_max; y++)
    {
        for (int x=x_min; x < x_max; x++)
        {
            Partition& part = GetPartition(x,y);

            for (auto i : part)
            {
                auto abounds = i->GetBounds();
                view.DrawRect(abounds.Inflate(-5), 170);
                bool collides = player.actor->CollisionCheck(i);
                if (collides)
                {
                    view.DrawRect(player.actorBounds & abounds, 57);
                }
            }
        }
    }

}
#endif

void GameServer::Update()
{
    commands.Clear();

    for(Partition& partition : partitions)
    {
        Rect<s16> bounds = partition.GetBounds();
        for(Actor* actor : partition.Reverse())
        {
            assert(actor);
            actor->Update();

            if( actor->IsDestroyed())
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
    PerformCollisionCheck();
    PerformPendingDeletes();

    BuildCommandBuffer(player, remotePlayer, commands);
    if(client)
    {
        BuildCommandBuffer(remotePlayer, player, clientCommands);
        clientCommands.Send(client);
        clientCommands.Clear();
    }
}

void GameServer::BuildCommandBuffer(PlayerInfo& thisPlayer, PlayerInfo& otherPlayer, CommandBuffer& commandBuffer)
{
    if(!thisPlayer.actor)
    {
        return;
    }

    const Vector<s16>& stageSize = stage.GetSize();
    Rect<s16> playerBounds = thisPlayer.actor->GetBounds();
    View view = View(stage, screen);

    // Update the viewpoint of the current player.
    // Don't snap it directly to the player's position, but have it lag slightly
    // based on the distance to the previous wiew point.
    Vector<s16> targetCamera = playerBounds.Center();
    Vector<s16> diff = targetCamera - thisPlayer.camera;

    // Take wrapping around the stage into account
    if (diff.x > stageSize.x / 2 )
    {
        diff.x = diff.x - stageSize.x;
    }
    else if ( diff.x < -stageSize.x / 2)
    {
        diff.x += stageSize.x;
    }

    if (diff.y > stageSize.y / 2)
    {
        diff.y = diff.y - stageSize.y;
    }
    else if (diff.y < -stageSize.y / 2)
    {
        diff.y += stageSize.y;
    }

    Vector<s16> moveDelta = diff / 20;

    // If we are really close, use the target position.
    if (moveDelta.IsZero())
    {
        thisPlayer.camera = targetCamera;
    }
    else
    {
        thisPlayer.camera = stage.WrapCoordinate(thisPlayer.camera+moveDelta);
    }
 
    view.SetCenterOffset(thisPlayer.camera);
    Vector<s16> otherPlayerPos = (otherPlayer.actor ? otherPlayer.actor->GetPosition() : Vector<s16>(-1,-1));

    commandBuffer.SetPlayerPositions(playerBounds.origin, otherPlayerPos);
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
    Array<Actor*> found;

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
                    for(Actor* other : GetPartition(x,y))
                    {
                        assert(other);
                        if (collider->CollisionCheck(other))
                        {
                            found.Append(other);
                        }
                    }
                }
            }

            for (Actor* collided : found)
            {
                if (!collided->IsDestroyed())
                {
                    collided->OnCollision(collider);
                    if (collided->IsDestroyed())
                    {
                        pendingDelete.Append(collided);
                    }
                }

                if (!collider->IsDestroyed())
                {
                    collider->OnCollision(collided);
                    if (collider->IsDestroyed())
                    {
                        pendingDelete.Append(collider);
                    }
                }
            }
            found.ClearFast();
        }
    }
}

void GameServer::SpawnFortress(const Level::FortressSpec& area)
{
    Base::CreateFort(*this, random, area);
}

void GameServer::SpawnEnemy(const Level::EnemySpec& enemy)
{
    AddActor(new Enemy(*this, imageSheet, random));
}

void GameServer::SpawnPlayer(const Level::SpawnPoint& point)
{
    if (player.actor)
    {
        player.actor->Destroy();
    }
    player.actor = new Player(*this, imageSheet, input, point.location, point.heading);
    AddActor(player.actor);
}

void GameServer::SpawnRemotePlayer(const Level::SpawnPoint& point)
{
    assert(client);

    if (remotePlayer.actor)
    {
        remotePlayer.actor->Destroy();
    }
    remotePlayer.actor = new Player(*this, imageSheet, clientInput, point.location, point.heading);
    AddActor(remotePlayer.actor);
}

void GameServer::SpawnMissile(const Vector<s16>& startPosition, const Direction& direction, int speed)
{
    AddActor(new Shot(*this, imageSheet, ImageSet::Missile, startPosition, direction, speed));
}

void GameServer::SpawnExplosion(const Vector<s16>& startPosition, const Direction& direction, int speed)
{
    AddActor(new Explosion(*this, imageSheet, startPosition, direction, speed));
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
        assert(actor->IsDestroyed());
        if(actor->collisionSourceMask != CollisionMask::None)
        {
            auto found = collisionSources.FindFirst([&actor](Actor* other){ return actor == other; });
            assert(found);
            found.Remove();
        }

        if(actor == player.actor)
        {
            player.actor = nullptr;
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

void GameServer::OnBaseDestroyed(Base* base)
{
    Background::GridPosition position = Background::WorldToGrid(base->GetPosition());
 
    background.ClearCell(position);
    if (client)
    {
        clientCommands.ClearBackgroundCell(position);
    }

    baseCount --;
    if (baseCount == 0)
    {
        // TODO: Delay loading of the next level for a bit
        LoadLevel();        
    }
}

void GameServer::OnBaseChanged(Base* base, u8 imageGroup, u8 imageIndex)
{
    Background::GridPosition position = Background::WorldToGrid(base->GetPosition());

    background.SetCell(base->GetPosition(), imageGroup, imageIndex);
    if(client)
    {
        clientCommands.SetBackgroundCell(position, imageGroup, imageIndex);
    }
}

void GameServer::AddBase(Base* base)
{
    AddActor(base);
    baseCount++;
}

void GameServer::LoadLevel(int levelIndex)
{
    if (levelIndex < 0)
    {
        currentLevel ++;
    }
    else
    {
        currentLevel = levelIndex;
    }

    if(currentLevel >= levels.Size())
    {
        currentLevel = 0;
    }
    const Level& level = levels[currentLevel];

    baseCount = 0;
    for(auto& fortress : level.fortresses)
    {
        SpawnFortress(fortress);
    }

    for(auto& enemySpec : level.enemies)
    {
        SpawnEnemy(enemySpec);
    }

    int localSpawnPoint = random.Get() % level.playerStarts.Size();
    SpawnPlayer(level.playerStarts[localSpawnPoint]);
    if(client)
    {
        int remoteSpawnPoint;
        do
        {
            remoteSpawnPoint = random.Get() % level.playerStarts.Size();
        } while (remoteSpawnPoint == localSpawnPoint);
        SpawnRemotePlayer(level.playerStarts[remoteSpawnPoint]);

        // Send updated level data to the client immediately
        // Block until the setup commands have been sent
        clientCommands.Send(client, true);
        clientCommands.Clear();
    }
}


void GameServer::Bind()
{
    Pause(); // If called from a different task, we have to disable updates while waiting
    client = network.WaitForClient();
    if(client)
    {
        client->Send("HI!", 3, 0);

        DEBUG("Waiting for greeting");
        u8 buffer[FRAME_BUFFER_SIZE];
        client->Receive(buffer, FRAME_BUFFER_SIZE, 0);
        // TODO Verify greeting
        readerTask = new NetworkReader(client, clientInput); 
    }
    Resume();
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