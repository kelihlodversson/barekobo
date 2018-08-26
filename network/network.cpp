#include <circle/util.h>
#include "network/network.h"

using namespace hfh3;

bool Network::Initialize()
{
    return netSubsystem.Initialize();
}

uint32_t Network::GetIPAddress()
{
    return *netSubsystem.GetConfig()->GetIPAddress();
}
