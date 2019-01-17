#pragma once
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>

#include "game/gameserver.h"
#include <climits>

namespace hfh3
{

    /** A GameServer subclass that generates random number of objects and
      * logs statistics to the serial port.
      */
    class PerfTester : public GameServer
    {
    public:
        PerfTester(MainLoop& inMainLoop, class Input& inInput, class Network& inNetwork);
        virtual ~PerfTester();

        virtual void Update() override;
        virtual void LoadLevel(int level=-1) override;
    private:

        struct Timer {
            unsigned actorUpdate;
            unsigned assignPartitions;
            unsigned buildCommandBuffer;
            int visibleActors;
        };

        void UpdateStats();

        void ClearStats();

        void LogStats();

        unsigned GetTicks()
        {
            return CTimer::Get()->GetClockTicks();
        }

        void InitTicks()
        {
            current = {0,0,0,0};
            frameStart = GetTicks();
        }

        unsigned frameCount;
        unsigned frameStart;
        Timer current;
        Timer sum;

        int actorCount;
    };
}
