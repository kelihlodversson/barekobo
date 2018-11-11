#include "network/beacon.h"
#include "util/log.h"

#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>

using namespace hfh3;

Beacon::Beacon()
    : task(new Task())
{
}

Beacon::~Beacon()
{
    if(task)
    {
        task->active = false;
        task = nullptr;
    }
}

Beacon::Task::Task()
    : active(true)
{}

Beacon::Task::~Task()
{
    DEBUG("Beacon::Task::~Task");
}

void Beacon::Task::Run()
{
    DEBUG("Beacon::Task::Run");
    CNetSubSystem* netSubsystem = CNetSubSystem::Get();
    assert(netSubsystem);

    CScheduler *scheduler = CScheduler::Get();
    CNetConfig *netConfig = netSubsystem->GetConfig();
    assert(netConfig);
    CSocket socket(netSubsystem, IPPROTO_UDP);

    CIPAddress broadcast;
    broadcast.SetBroadcast();
    if (socket.Connect(broadcast, (u16)BEACON_PORT) != 0)
    {
        ERROR("Cannot set up beacon UDP socket.");
        return;
    }

    socket.SetOptionBroadcast(true);

    beacon_data_t payload = *netConfig->GetIPAddress();
    while (active)
    {
        int res = socket.Send(&payload, sizeof(beacon_data_t), MSG_DONTWAIT);
        DEBUG("socket.Send: %d", res);
        scheduler->Sleep(1);
    }

    payload = 0;
    DEBUG("Closing down beacon: Sending sign off packet.");
    int res = socket.Send(&payload, sizeof(beacon_data_t), MSG_DONTWAIT);
    DEBUG("socket.Send: %d", res);
    scheduler->Sleep(1);
    DEBUG("Exiting task");

}
