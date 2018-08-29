#include <circle/sched/scheduler.h>
#include <circle/net/socket.h>
#include <circle/net/ipaddress.h>
#include <circle/net/in.h>

#include "network/beacon.h"
#include "util/log.h"


using namespace hfh3;

Beacon::Beacon(CNetSubSystem *inNetSubsystem)
    : netSubsystem(inNetSubsystem)
    , listener(this)
    , broadcaster(this)
{

}

Beacon::~Beacon()
{

}

Beacon::Broadcaster::Broadcaster(Beacon* inBeacon)
    : beacon(inBeacon)
{

}


Beacon::Broadcaster::~Broadcaster()
{

}

void Beacon::Broadcaster::Run()
{
    CScheduler *scheduler = CScheduler::Get();
    CNetConfig *netConfig = beacon->netSubsystem->GetConfig();
    assert(netConfig);
    CSocket socket(beacon->netSubsystem, IPPROTO_UDP);

    CIPAddress broadcast;
    broadcast.SetBroadcast();
    if (socket.Connect(broadcast, (u16)BEACON_PORT) != 0)
    {
        ERROR("Cannot set up beacon UDP socket.");
        return;
    }

    socket.SetOptionBroadcast(true);

    beacon_data_t payload = *netConfig->GetIPAddress();
    while (true)
    {
        int res = socket.Send(&payload, sizeof(beacon_data_t), MSG_DONTWAIT);
        DEBUG("socket.Send: %d", res);
        scheduler->Sleep(1);
    }
}

Beacon::Listener::Listener(Beacon* inBeacon)
    : beacon(inBeacon)
{

}

Beacon::Listener::~Listener()
{

}


void Beacon::Listener::Run()
{
    CScheduler *scheduler = CScheduler::Get();
    CNetConfig *netConfig = beacon->netSubsystem->GetConfig();
    assert(netConfig);
    CSocket socket(beacon->netSubsystem, IPPROTO_UDP);
    if (socket.Bind((u16)BEACON_PORT) != 0)
    {
        ERROR("Cannot bind to beacon UDP socket.");
        return;
    }
    socket.SetOptionBroadcast(true);

    beacon_data_t payload;

    while(true)
    {
        int res = socket.Receive(&payload, sizeof(beacon_data_t), 0);
        if (res > 0)
        {
            DEBUG("Received beacon packet: %u.%u.%u.%u",
                payload & 0xff,
                (payload >> 8) & 0xff,
                (payload >> 16) & 0xff,
                (payload >> 14) & 0xff
            );
        }
        else
        {
            DEBUG("Error receiving data: %d",res);
        }
        scheduler->Yield();
    }
}
