#pragma once
#include "circle/net/socket.h"
#include "circle/sched/scheduler.h"
#include "circle/sched/task.h"


#include "game/world.h"

namespace hfh3
{

    /** A thin client for submitting input events to the server and
      * performing commands sent from the server.
      */
    class GameClient : public World
    {
    public:
        GameClient(ScreenManager& inScreen, class Input& inInput, class Network& inNetwork);
        virtual ~GameClient();

        void Connect();

        virtual void Update() override;
        virtual void Draw() override;
    private:
    
        // Due to how the socket framework schedules transfers,
        // the client sometimes needs an Idle task to yield to while waiting for data.
        class Idle : public CTask
        {
        public:
            virtual void Run() override
            {
                CScheduler *scheduler = CScheduler::Get();

                while(true)
                {
                    scheduler->Yield();
                }
            }
        };
        CSocket* server;
        Idle idleTask;

    };
}
