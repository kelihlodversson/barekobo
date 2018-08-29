#pragma once
#include <circle/types.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>

namespace hfh3
{
    class Network
    {
    public:
        Network();
        ~Network();

        bool Initialize();
        u32 GetIPAddress();
    private:
        CScheduler      scheduler;      // The scheduler is required by the net subsystem
        CNetSubSystem   netSubsystem;

        class Beacon    *beacon;
    };
}
