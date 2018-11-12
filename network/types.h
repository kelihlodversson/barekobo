#pragma once
#include <circle/types.h>

namespace hfh3
{
    /* Various constants and type definitions for IP addresses */
    using ipv4_address_t = u32;
    using ipv4_port_t = u16;

    using beacon_data_t = u32;

    /* We use the same port number for the game data and the beacon, as the former is 
     * over TCP and the latter UDP */
    static const ipv4_port_t GAME_PORT = 12345;
    static const ipv4_port_t BEACON_PORT = 12345;

}