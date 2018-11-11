#pragma once

#include <circle/types.h>
#include <circle/sched/task.h>


#define BEACON_PORT 12345

namespace hfh3
{
    /**
      * Simple broadcast announcer that announces this node's IP address on
      * the local network.
      */
    class Beacon 
    {
    public:
        using beacon_data_t = u32;
        Beacon();
        ~Beacon();

    private:

        // Inner class that does the actual work
        class Task : public CTask
        {
            public:
            Task();
            virtual ~Task();

            // Main entry point for the Broadcaster task
            virtual void Run() override;
            volatile bool active;
        };

        // All CTask instances are owned by the scheduler and should only be 
        // destroyed by returning from Run().
        // We must be careful to only notify the task that it should exit
        // when the outer object is deleted.
        Task* task;
    };
}
