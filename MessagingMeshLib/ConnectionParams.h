#pragma once
#include <string>

namespace MessagingMesh
{
    // Connection parameters.
    struct ConnectionParams
    {
        // The gateway host or IP address.
        std::string GatewayHost;

        // The gateway port.
        int GatewayPort;

        // The messaging-mesh service to join.
        std::string Service;
    };
} // namespace


