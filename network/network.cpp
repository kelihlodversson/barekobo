#include <circle/util.h>
#include <circle/net/in.h>
#include <circle/net/ipaddress.h>

#include "network/network.h"
#include "network/beacon.h"
#include "util/log.h"

using namespace hfh3;

bool Network::Initialize()
{
    if(!netSubsystem.Initialize())
    {
        return false;
    }

    return true;
}

u32 Network::GetIPAddress()
{
    return *netSubsystem.GetConfig()->GetIPAddress();
}

Network::Network()
{
}

// Listens for a single client connection and returns it.
CSocket* Network::WaitForClient()
{
	CSocket listener(&netSubsystem, IPPROTO_TCP);
    if(listener.Bind(GAME_PORT) < 0)
    {
        ERROR("Cannot bind to port %u", GAME_PORT);
        return nullptr;
    }

    if(listener.Listen(1) < 0)
    {
        ERROR("Could not listen for connections");
        return nullptr;
    }
    
    CIPAddress remote_ip;
    u16 remote_port;
    CSocket* connection;
    do
    {
        connection = listener.Accept(&remote_ip, &remote_port);
        if (!connection)
        {
            WARN("Could not accept connection. Waiting for an another attempt...");
            continue;
        }
    } while(0);

    return connection;
}

// Connects to the server and returns the connection object.
CSocket* Network::ConnectToServer(ipv4_address_t address, ipv4_port_t port)
{
    CSocket* connection = new CSocket(&netSubsystem, IPPROTO_TCP);
    CIPAddress circleAddress(address);
    if (connection->Connect(circleAddress, port) < 0)
    {
        delete connection;
        return nullptr;
    }
    return connection;
}

Network::~Network()
{
}
