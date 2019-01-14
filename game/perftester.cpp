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
    INFO("%%[BEGIN TEST RUN]");
    INFO("\\pgfplotstableread{");
    INFO("actor_count visible update partitioning build_command_list exec_command_list other_game_loop frame_copy total_frame_time fps");
}

PerfTester::~PerfTester()
{
    INFO("}\\table_%s%s%s%s%s",
        CONFIG_GPU_PAGE_FLIPPING?"pageflip":CONFIG_DMA_FRAME_COPY?"dma":"memcpy",
        CONFIG_NEON_RENDER?"_neon":"",
        CONFIG_USE_ITEM_POOL?"_itemPool":"",
        CONFIG_OWN_MEMSET?"_customMemSet":"",
        CONFIG_PRERENDER_STARFIELD?"_prerender":""
    );
    INFO("%%[END OF TEST RUN]");
}

static const int FRAMES_PER_TEST = 60 * 30; // Run each test for 1800 frames or at least 30 seconds (longer if we miss frames.)
static const int ACTOR_INCREMENT = 1000; // Number of objects to add each test.
static const int MAX_ACTOR_COUNT = 20000; // The test will exit after reaching this number of actors in the level.

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
}

void PerfTester::Render()
{
    GameServer::Render();
    current.render = GetTicks();

    UpdateStats();
}



void PerfTester::LoadLevel(int level)
{
    ClearStats();
    screen.ClearTimers();

    if(level >= 0)
    {
        for(int i=0; i<ACTOR_INCREMENT; i++)
        {
            SpawnEnemy(Random::Instance().GetVector<s16>());
        }
        actorCount += ACTOR_INCREMENT;
    }
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

#define AVG(field, pfx) double(pfx ## sum.field) / frameCount / CLOCKHZ * 1000000.0

void PerfTester::LogStats()
{
    ScreenManager::Timer screen_sum, screen_min, screen_max;
    screen.GetTimers(screen_sum, screen_min, screen_max);

    double avg_update = AVG(actorUpdate,);
    double avg_partitions = AVG(assignPartitions,);
    double avg_build = AVG(buildCommandBuffer,);
    double avg_render = AVG(render,);
    double total = avg_update + avg_partitions + avg_build + avg_render;
    double other_main_loop = AVG(gameTicks, screen_) - total;

    INFO("\t%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
        actorCount,
        double(sum.visibleActors) / frameCount,
        avg_update,
        avg_partitions,
        avg_build,
        avg_render,
        other_main_loop,
        AVG(presentTicks, screen_),
        AVG(ticksPerFrame, screen_),
        double(CLOCKHZ) / (double(screen_sum.ticksPerFrame) / frameCount)
    );
}