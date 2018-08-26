#pragma once
#include <stdint.h>
#include <circle/types.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>

namespace hfh3
{
    class Network
    {
    public:
        bool Initialize();
        uint32_t GetIPAddress();
    private:
        CScheduler      scheduler;      // The scheduler is required by the net subsystem
        CNetSubSystem   netSubsystem;
    };
}
