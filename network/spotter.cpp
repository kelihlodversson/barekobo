#include <circle/net/netsubsystem.h>
#include <circle/sched/scheduler.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>

#include "network/spotter.h"
#include "util/log.h"


using namespace hfh3;

Spotter::~Spotter()
{
    if(task)
    {
        task->active = false;
        task = nullptr;
    }
}

Spotter::Task::~Task()
{
    DEBUG("Spotter::Task::~Task");
}

void Spotter::Task::Run()
{
    DEBUG("Beacon::Task::Run");
    CNetSubSystem* netSubsystem = CNetSubSystem::Get();
    assert(netSubsystem);

    CScheduler *scheduler = CScheduler::Get();
    CNetConfig *netConfig = netSubsystem->GetConfig();
    assert(netConfig);
    CSocket socket(netSubsystem, IPPROTO_UDP);
    if (socket.Bind((u16)BEACON_PORT) != 0)
    {
        ERROR("Cannot bind to beacon UDP socket.");
        return;
    }
    socket.SetOptionBroadcast(true);

    beacon_data_t payload;

    CIPAddress remoteAddress;
    u16 dummy;

    while(active)
    {
        int res = socket.ReceiveFrom(&payload, sizeof(beacon_data_t), 0,
            &remoteAddress, &dummy);
        if (res > 0)
        {
            UpdateHosts(remoteAddress, payload);
        }
        else
        {
            DEBUG("Error receiving data: %d",res);
        }
        scheduler->Yield();
    }
}

void Spotter::Task::UpdateHosts(const CIPAddress& address, bool available)
{
    auto existing = knownHosts.FindFirst([&](Host& host){ return host.address == address; });
    if (existing)
    {
        existing->SetValid(available);
    }
    else if(available)
    {
        // Insert the new host in sorted order
        auto next = knownHosts.FindFirst([&](Host& host){ return host.address >= address; });
        if (next)
        {
            next.InsertBefore(address, true);
        }
        else
        {
            knownHosts.Append(address, true);
        }
    }
    callback();
}

