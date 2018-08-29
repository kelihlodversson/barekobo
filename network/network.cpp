#include <circle/util.h>
#include "network/network.h"
#include "network/beacon.h"

using namespace hfh3;

bool Network::Initialize()
{
    if(!netSubsystem.Initialize())
    {
        return false;
    }

    beacon = new Beacon(&netSubsystem);

    return true;
}

u32 Network::GetIPAddress()
{
    return *netSubsystem.GetConfig()->GetIPAddress();
}

Network::Network()
    : beacon(nullptr)
{
}

Network::~Network()
{
    if (beacon)
    {
        delete beacon;
        beacon = nullptr;
    }
}
