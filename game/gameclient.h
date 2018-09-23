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

    private:

        class NetworkReader : public CTask
        {
        public:
            NetworkReader(CSocket* inServer, CommandBuffer& inCommandBuffer)
                : server(inServer)
                , commands(inCommandBuffer)
            {}

            virtual void Run() override;

        private:
            CSocket* server;
            CommandBuffer& commands;
        };

        u8 lastInputState;
        CSocket* server;
        NetworkReader* readerTask;
    };
}
