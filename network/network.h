#pragma once
#include "network/types.h"

#include <circle/types.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>

namespace hfh3
{
    class Network
    {
    public:


        Network();
        ~Network();

        bool Initialize();
        u32 GetIPAddress();

        // Listens for a single client connection and returns it.
        CSocket* WaitForClient();

        // Connects to the server and returns the connection object.
        CSocket* ConnectToServer(ipv4_address_t address, ipv4_port_t port = GAME_PORT);

    private:
        CScheduler      scheduler;      // The scheduler is required by the net subsystem
        CNetSubSystem   netSubsystem;

        friend class Beacon;
        friend class Spotter;
    };
}
