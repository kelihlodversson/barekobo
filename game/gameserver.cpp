#include "game/gameserver.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"
#include "util/random.h"

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
#include "game/commandlist.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <limits.h>

using namespace hfh3;

GameServer::GameServer(MainLoop& inMainLoop, class Input& inInput, Network& inNetwork)
    : World(inMainLoop, inInput, inNetwork)
    , partitionSize(stage.GetSize() / partitionGridCount)
    , player({stage.GetSize()/2, stage.GetSize()/2})
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

GameServer::PlayerInfo::PlayerInfo(const Vector<s16>& initialCamera)
    : actor(nullptr)
    , camera(initialCamera)
    , score(0)
    , lives(2)
{
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
        readerTask->active = false;
        readerTask = nullptr;
    }

    
    // Closing the client connection will be handled by the NetworkReader
    if(client)
    {
        client = nullptr;
    }
}

GameServer::NetworkReader::~NetworkReader()
{
    if(connection)
    {
        delete connection;
        connection = nullptr;
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
    if(baseCount == 0 && loadLevelDelay-- == 0)
    {
        LoadLevel();
    }

    // Exit the game if no players are left
    if(player[0].lives == 0 && (!client || player[1].lives == 0) && loadLevelDelay-- == 0)
    {
        mainLoop.DestroyClient(this);
        return;
    }

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

    BuildCommandBuffer(player[0], player[1], commands);
    if(client)
    {
        BuildCommandBuffer(player[1], player[0], clientCommands);
        clientCommands.Send(client);
        clientCommands.Clear();
    }
}

void GameServer::BuildCommandBuffer(PlayerInfo& thisPlayer, PlayerInfo& otherPlayer, CommandList& commandBuffer)
{
    
    const Vector<s16>& stageSize = stage.GetSize();
    View view = View(stage, screen);

    if (thisPlayer.actor)
    {
        // Update the viewpoint of the current player.
        // Don't snap it directly to the player's position, but have it lag slightly
        // based on the distance to the previous wiew point.
        Rect<s16> playerBounds = thisPlayer.actor->GetBounds();
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

        static const int cameraLag = 20;
        Vector<s16> moveDelta = Vector<s16>((Vector<s32>(diff) * (cameraLag-1)) / cameraLag);
        thisPlayer.camera = stage.WrapCoordinate(targetCamera-moveDelta);
        Vector<s16> otherPlayerPos = (otherPlayer.actor ? otherPlayer.actor->GetPosition() : Vector<s16>(-1,-1));
        commandBuffer.SetPlayerPositions(playerBounds.origin, otherPlayerPos);
    }
 
    view.SetCenterOffset(thisPlayer.camera);

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
    Base::CreateFort(*this, area);
}

void GameServer::SpawnEnemy(const Vector<s16>& location)
{
    Actor* enemy = new Enemy(*this, imageSheet);
    enemy->SetPosition(location);
    enemy->SetDestructionHandler([=]()
    {
        UpdateScore(enemy->GetKiller(), enemy->GetScore());
    });
    AddActor(enemy);
}

void GameServer::SpawnPlayer(int index, const Level::SpawnPoint& point)
{
    assert(index >= 0 && index < maxPlayerCount);
    if (player[index].actor)
    {
        player[index].actor->Destroy();
    }
    player[index].actor = new Player(*this, index, imageSheet, index==1?clientInput:input, point.location, point.heading);
    AddActor(player[index].actor);
    player[index].actor->SetDestructionHandler([=]()
    {
        OnPlayerDestroyed(index);
    });
}

void GameServer::SpawnMissile(int playerIndex, Direction direction, int speed)
{
    assert(playerIndex >= 0 && playerIndex < maxPlayerCount && player[playerIndex].actor);
    
    Vector<s16> startPosition = stage.WrapCoordinate(player[playerIndex].actor->GetPosition() + direction.ToDelta(maxActorSize));
    AddActor(new Shot(*this, imageSheet, ImageSet::Missile, startPosition, direction, speed, playerIndex));
}

void GameServer::SpawnShot(const Vector<s16>& startPosition, const Direction& direction, int speed)
{
    AddActor(new Shot(*this, imageSheet, ImageSet::MiniShot, startPosition, direction, speed));
}

Actor* GameServer::SpawnExplosion(const Vector<s16>& startPosition, const Direction& direction, int speed)
{
    return AddActor(new Explosion(*this, imageSheet, startPosition, direction, speed));
}

Actor* GameServer::AddActor(Actor* newActor)
{
    if(newActor->collisionSourceMask != CollisionMask::None)
    {
        collisionSources.Prepend(newActor);
    }
    needsNewPartition.Append(newActor);
    return newActor;
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

        for (PlayerInfo& p : player)
        {
            if(actor == p.actor)
            {
                p.actor = nullptr;
            }
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

void GameServer::ClearLevel()
{
    pendingDelete.Clear();
    collisionSources.Clear();

    for (PlayerInfo& p : player)
    {
        p.actor = nullptr;
    }   

    for(Partition& partition : partitions)
    {
        for(Actor* actor : partition.Reverse())
        {
            assert(actor);
            delete actor;
        }
        partition.Clear();
    }
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

Actor* GameServer::FindPlayer(const Vector<s16>& position, int radius, Vector<s16>& outDelta)
{
    int sqRadius = radius*radius;

    int minSquared = sqRadius;
    Actor* found = nullptr;

    for(PlayerInfo& p : player)
    {
        if(!p.actor)
        {
            continue;
        }

        // Use 32 bits when calculating the square magnitude to avoid overflows
        Vector<s32> delta (p.actor->GetPosition() - position);
        int sqMagnitude = delta.SqrMagnitude();
        if (sqMagnitude < minSquared)
        {
            found = p.actor;
            minSquared = sqMagnitude;
            outDelta = Vector<s16>(delta);
        }

    }

    return found;
}

void GameServer::UpdateScore(int playerIndex, int scoreChange)
{
    if (playerIndex >= 0 && playerIndex < 2 && scoreChange)
    {
        player[playerIndex].score += scoreChange;
        minimap->SetPlayerScore(playerIndex, player[playerIndex].score);
        if (client)
        {
            clientCommands.SetPlayerScore(playerIndex, player[playerIndex].score);
        }
    }
}

void GameServer::OnPlayerDestroyed(int playerIndex)
{
    assert(playerIndex >=0 && playerIndex < maxPlayerCount);
    assert(player[playerIndex].actor);
    
    player[playerIndex].lives --;
    Player* p = player[playerIndex].actor;
    Actor* explosion = SpawnExplosion(p->GetPosition(), p->GetDirection(), p->GetSpeed());

    if (player[playerIndex].lives > 0)
    {
        explosion->SetDestructionHandler([=]()
        {
            auto& spawnPoints = levels[currentLevel].playerStarts;
            SpawnPlayer(playerIndex, spawnPoints[Rand() % spawnPoints.Size()]);
        });
        overlay->SetMessage(Message::GetReady, currentLevel, 150);
    }
    else
    {
        overlay->SetMessage(Message::GameOver, currentLevel, -1);
    }


    minimap->SetPlayerLives(playerIndex, player[playerIndex].lives);
    if (client)
    {
        clientCommands.SetPlayerLives(playerIndex, player[playerIndex].lives);
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

    for(Partition& partition : partitions)
    {
        for(Actor* actor : partition.Reverse())
        {
            assert(actor);
            if( !actor->IsDestroyed())
            {
                actor->OnBaseDestroyed(baseCount);
            }
        }
    }

    if (baseCount == 0)
    {
        overlay->SetMessage(Message::LevelCleared, currentLevel, loadLevelDelay);
    }
    UpdateScore(base->GetKiller(), base->GetScore());
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
    base->SetDestructionHandler([=]()
    {
        OnBaseDestroyed(base);
    });
}

void GameServer::LoadLevel(int levelIndex)
{
    // Initialize scores and lives if this is the first level loaded
    if (currentLevel < 0)
    {
        if (!client)
        {
            player[1].lives = -1;
        }
        for(int i = 0; i < 2; i++)
        {
            minimap->SetPlayerLives(i, player[i].lives);
            minimap->SetPlayerScore(i, player[i].score);
            if (client)
            {
                clientCommands.SetPlayerLives(i, player[i].lives);
                clientCommands.SetPlayerScore(i, player[i].score);
            }
        }
    }
    // Remove all objects from the current level
    ClearLevel();

    loadLevelDelay = 60 * 5; // Wait 5 seconds before loading the next level
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

    Array<Level::SpawnPoint> spawnPoints (level.playerStarts);
    int playerCount = client?2:1;
    for (int i = 0; i <playerCount; i++)
    {
        auto spawnPoint = spawnPoints.Pull(Rand() % spawnPoints.Size());
        SpawnPlayer(i, spawnPoint);
    }

    overlay->SetMessage(Message::GetReady, currentLevel, 150);

    if(client)
    {
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
    while(active)
    {
        u8 buffer[FRAME_BUFFER_SIZE];
        int count = connection->Receive(buffer, FRAME_BUFFER_SIZE, MSG_DONTWAIT);

        if(!active) // remoteInput may be invalid if active is false
            break;

        if(count > 0)
        {
            for(int i = 0; i < count; i++)
            {
                remoteInput.SetInputState(buffer[i]);
            }
        }
        // TODO: add error handling when count is negative
        else
        {
            // When there is no data ready, sleep for 60th of a second
            // before trying again. This will also yield control to other threads
            scheduler->MsSleep(16); 
        }
    }
}