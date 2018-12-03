#pragma once
#include <circle/net/socket.h>
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>

#include "network/types.h"
#include "game/world.h"

namespace hfh3
{

    /** A thin client for submitting input events to the server and
      * performing commands sent from the server.
      */
    class GameClient : public World
    {
    public:
        GameClient(MainLoop& inMainLoop, class Input& inInput, class Network& inNetwork);
        virtual ~GameClient();

        void Connect(ipv4_address_t address, ipv4_port_t port = GAME_PORT);

        virtual void Update() override;

    private:

        class NetworkReader : public CTask
        {
        public:
            NetworkReader(CSocket* inServer, CommandList& inCommandBuffer, GameClient* me)
                : server(inServer)
                , commands(inCommandBuffer)
                , outer(me)
            {}

            virtual void Run() override;
            volatile bool active;
        private:
            CSocket* server;
            CommandList& commands;
            GameClient* outer;
        };

        u8 lastInputState;
        CSocket* server;
        NetworkReader* readerTask;
        volatile bool active;
    };
}
