#include "game/perftester.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"
#include "util/random.h"

using namespace hfh3;

PerfTester::PerfTester(MainLoop& inMainLoop, class Input& inInput, Network& inNetwork)
    : GameServer(inMainLoop, inInput, inNetwork)
    , frameCount(0)
    , actorCount(0)
{}

PerfTester::~PerfTester()
{}

static const int FRAMES_PER_TEST = 60 * 10; // Run each test for 10 seconds
static const int OBJECT_COUNT = 1000; // Number of objects to add each test

void PerfTester::Update()
{
    commands.Clear();
    if(frameCount == FRAMES_PER_TEST)
    {
        LogStats();
        LoadLevel();
    }
    InitTicks();

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
    current.actorUpdate = GetTicks();

    AssignPartitions();
    current.assignPartitions = GetTicks();

    current.visibleActors = BuildCommandBuffer(player[0], player[1], commands);
    current.buildCommandBuffer = GetTicks();
}

void PerfTester::Render()
{
    GameServer::Render();
    current.render = GetTicks();

    UpdateStats();
}



void PerfTester::LoadLevel(int /*unused*/)
{
    ClearStats();
    screen.ClearTimers();

    for(int i=0; i<OBJECT_COUNT; i++)
    {
        SpawnEnemy(Random::Instance().GetVector<s16>());
    }
    actorCount += OBJECT_COUNT;
}

#define UPDATE_SUM(field) sum.field += current.field
#define UPDATE_MAX(field) if (max.field < current.field) max.field = current.field
#define UPDATE_MIN(field) if (min.field > current.field) min.field = current.field

void PerfTester::UpdateStats()
{

    // Convert current absolute time stamps to relative by subtracting the previous 
    // stamp from the next one:
    current.render -= current.buildCommandBuffer;
    current.buildCommandBuffer -= current.assignPartitions;
    current.assignPartitions -= current.actorUpdate;
    current.actorUpdate -= frameStart;

    UPDATE_SUM(actorUpdate);
    UPDATE_SUM(assignPartitions);
    UPDATE_SUM(buildCommandBuffer);
    UPDATE_SUM(visibleActors);
    UPDATE_SUM(render);

    UPDATE_MAX(actorUpdate);
    UPDATE_MAX(assignPartitions);
    UPDATE_MAX(buildCommandBuffer);
    UPDATE_MAX(visibleActors);
    UPDATE_MAX(render);
    
    UPDATE_MIN(actorUpdate);
    UPDATE_MIN(assignPartitions);
    UPDATE_MIN(buildCommandBuffer);
    UPDATE_MIN(visibleActors);
    UPDATE_MIN(render);

    frameCount++;
}

#define LOG_FIELD(name, field, pfx) \
    INFO("  " name ": %.2fus (min: %.0fus, max: %.0fus)", \
        double(pfx ## sum.field) / frameCount / CLOCKHZ * 1000000.0, \
        double(pfx ## min.field) / CLOCKHZ * 1000000.0, \
        double(pfx ## max.field) / CLOCKHZ * 1000000.0) 

void PerfTester::LogStats()
{
    INFO("[BEGIN STAT] Actor count: %d", actorCount);
    INFO("Actors on-screen: %.2f (min: %d, max: %d)", 
        double(sum.visibleActors) / frameCount, min.visibleActors, max.visibleActors);

    LOG_FIELD("Updating Actors", actorUpdate, );
    LOG_FIELD("Assign Partitions", assignPartitions,);
    LOG_FIELD("Build Command Buffer", buildCommandBuffer,);
    LOG_FIELD("Executing Command Buffer", render,);

    ScreenManager::Timer screen_sum, screen_min, screen_max;
    int screen_frames = screen.GetTimers(screen_sum, screen_min, screen_max);
    LOG_FIELD("Game time", gameTicks, screen_);
    LOG_FIELD("Flip time", presentTicks, screen_);
    LOG_FIELD("Frame time", ticksPerFrame, screen_);
    INFO("[END STAT] Frame count: %d. Screen frames: %d", frameCount, screen_frames);
}