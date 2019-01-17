#include "game/perftester.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"
#include "util/random.h"
#include "config.h"

using namespace hfh3;

PerfTester::PerfTester(MainLoop& inMainLoop, class Input& inInput, Network& inNetwork)
    : GameServer(inMainLoop, inInput, inNetwork)
    , frameCount(UINT_MAX)
    , actorCount(0)
{
    INFO("%%[BEGIN TEST RUN %s%s%s%s%s]",
        CONFIG_GPU_PAGE_FLIPPING?"pageflip":CONFIG_DMA_PARALLEL?"dma2":CONFIG_DMA_FRAME_COPY?"dma1":"memcpy",
        CONFIG_NEON_RENDER?"_neon":"",
        CONFIG_USE_ITEM_POOL?"_itemPool":"",
        CONFIG_OWN_MEMSET?"_customMemSet":"",
        CONFIG_PRERENDER_STARFIELD?"_prerender":""
    );
    INFO("actorCount visible update render postRender otherGameLoop present totalFrameTime updateActors updatePartition renderPrepare fps");
}

PerfTester::~PerfTester()
{
    INFO("%%[END OF TEST RUN]");
}

static const int FRAMES_PER_TEST = 60 * 60; // Run each test for 3600 frames or at least 60 seconds (longer if we miss frames.)
static const int ACTOR_INCREMENT = 2000;    // Number of objects to add each test.
static const int MAX_ACTOR_COUNT = 14000;   // The test will exit after reaching this number of actors in the level.

void PerfTester::Update()
{
    // Initial level load
    if (frameCount == UINT_MAX)
    {
        LoadLevel();
    }
    // Update stats after running the preset amount of frames
    else if(frameCount == FRAMES_PER_TEST)
    {
        LogStats();

        if(actorCount >= MAX_ACTOR_COUNT)
        {
            mainLoop.DestroyClient(this);
            return;
        }
        else
        {
            LoadLevel(1);
        }
    }
    InitTicks();
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
    current.actorUpdate = GetTicks();

    AssignPartitions();
    current.assignPartitions = GetTicks();

    current.visibleActors = BuildCommandBuffer(player[0], player[1], commands);
    current.buildCommandBuffer = GetTicks();

    UpdateStats();
}

void PerfTester::LoadLevel(int level)
{
    ClearStats();

    if(level >= 0)
    {
        for(int i=0; i<ACTOR_INCREMENT; i++)
        {
            SpawnEnemy(Random::Instance().GetVector<s16>());
        }
        actorCount += ACTOR_INCREMENT;
    }
}

void PerfTester::ClearStats()
{
    screen.ClearTimers();
    mainLoop.ClearTimers();
    sum = {0,0,0,0};
    frameCount = 0;
}

#define UPDATE_SUM(field) sum.field += current.field

void PerfTester::UpdateStats()
{
    // Convert current absolute time stamps to relative by subtracting the previous 
    // stamp from the next one:
    current.buildCommandBuffer -= current.assignPartitions;
    current.assignPartitions -= current.actorUpdate;
    current.actorUpdate -= frameStart;

    UPDATE_SUM(actorUpdate);
    UPDATE_SUM(assignPartitions);
    UPDATE_SUM(buildCommandBuffer);
    UPDATE_SUM(visibleActors);
    frameCount++;
}

#define AVG(val) double(val) / frameCount / CLOCKHZ * 1000000.0

void PerfTester::LogStats()
{
    ScreenManager::Timer screen_sum;
    MainLoop::Timer mainLoop_sum;
    unsigned screenFrame = screen.GetTimers(screen_sum);
    unsigned mainLoopFrame = mainLoop.GetTimers(mainLoop_sum);
    if(!(frameCount == mainLoopFrame && frameCount == screenFrame))
    {
        ERROR("frameCount = %u, mainLoopFrame = %u, screenFrame = %u",
        frameCount, mainLoopFrame, screenFrame);
        assert(false);
    }

    double avg_actorUpdate = AVG(sum.actorUpdate);
    double avg_partitions = AVG(sum.assignPartitions);
    double avg_build = AVG(sum.buildCommandBuffer);
    double avg_update = AVG(mainLoop_sum.update);
    double avg_render = AVG(mainLoop_sum.render);
    double avg_postRender = AVG(mainLoop_sum.postRender);
    unsigned mainLoop_total = mainLoop_sum.update + mainLoop_sum.render + mainLoop_sum.postRender;
    double other_main_loop = AVG(screen_sum.gameTicks - mainLoop_total);

    INFO("%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
        actorCount,
        double(sum.visibleActors) / frameCount,
        avg_update,
        avg_render,
        avg_postRender,
        other_main_loop,
        AVG(screen_sum.presentTicks),
        AVG(screen_sum.ticksPerFrame),
        avg_actorUpdate,
        avg_partitions,
        avg_build,
        double(CLOCKHZ) / (double(screen_sum.ticksPerFrame) / frameCount)
    );
}