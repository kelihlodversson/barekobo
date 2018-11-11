#pragma once
#include "network/beacon.h"
#include "util/callback.h"
#include "util/list.h"
#include <circle/types.h>
#include <circle/sched/task.h>

#include <limits.h>

#define BEACON_PORT 12345

namespace hfh3
{
    /**
      * Simple broadcast announcer that announces this node's IP address on
      * the local network and listens for broadcast from others.
      */
    class Spotter
    {
    public:
        using beacon_data_t = Beacon::beacon_data_t;

        template<typename ...Args>
        Spotter(Args&&... args)
            : task(new Task(args...))
        {}

        ~Spotter();


        class Host 
        {
        public:
            static const int MAX_AGE = 4;
            static const int INVALID_IP = 0;

            const char* GetIpString() 
            {
                if(ipString.GetLength() == 0)
                {
                    address.Format(&ipString);
                }
                return (const char*)ipString;
            }

            void GetIpAddress(CIPAddress& outAddress) const
            {
                outAddress.Set(address);
            }

            void SetValid(bool isValid)
            {
                lastSeen = isValid?GetTime():INT_MIN;
            }

            bool IsValid()
            {
                return lastSeen >= GetTime() - MAX_AGE;
            }


            Host() 
                : address(reinterpret_cast<const u8*>(&INVALID_IP))
                , lastSeen(INT_MIN)
            {}

            Host(Host& other)
                : address(other.address)
                , lastSeen(other.lastSeen)
            {}

            Host(const CIPAddress& inAddress, bool isValid)
                : address(inAddress)
                , lastSeen(isValid?GetTime():INT_MIN)
            {}
        private:

            static int GetTime()
            {
                return CTimer::Get()->GetTime();
            }

            CIPAddress address;
            CString ipString;
            int lastSeen;

            friend Spotter;
        };

        using Hosts = List<Host>;

        template<typename F>
        void ForEachHost(F functor)
        {
            assert(task);
            for(Host& host : task->knownHosts)
            {
                if(host.IsValid())
                {
                    functor(host);
                }
            }
        }

    private:

        // Inner class that does the actual work
        class Task : public CTask
        {
            public:
            template<typename ...Args>
            Task(Args&&... args)
                : callback(args...)
                , active(true)
            {}

            virtual ~Task();

            // Main entry point for the Broadcaster task
            virtual void Run() override;

            void UpdateHosts(const CIPAddress& host, bool available);
          
            Callback<void()> callback;
            volatile bool active;
            Hosts knownHosts;
        };

        // All CTask instances are owned by the scheduler and should only be 
        // destroyed by returning from Run().
        // We must be careful to only notify the task that it should exit
        // when the outer object is deleted.
        Task* task;

    };
}
