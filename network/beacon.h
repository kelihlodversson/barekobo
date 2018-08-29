#pragma once
#include <circle/types.h>
#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>


#define BEACON_PORT 12345

namespace hfh3
{
    /**
      * Simple broadcast announcer that announces this node's IP address on
      * the local network and listens for broadcast from others.
      */
    class Beacon
    {
        typedef u32 beacon_data_t;
    public:
        Beacon(CNetSubSystem *inNetSubsystem);
        ~Beacon();

    private:
        class Listener : public CTask
        {
        public:
            Listener(Beacon *inBeacon);
            virtual ~Listener();

            // Main entry point for the Listener task
            virtual void Run() override;
        private:
            class Beacon* beacon;
        };

        class Broadcaster : public CTask
        {
        public:
            Broadcaster(Beacon *inBeacon);
            virtual ~Broadcaster();

            // Main entry point for the Broadcaster task
            virtual void Run() override;
        private:
            class Beacon* beacon;
        };

        CNetSubSystem *netSubsystem;

        Listener listener;
        Broadcaster broadcaster;

        friend Listener;
        friend Broadcaster;
    };
}
