#pragma once
#include <string>
#include <vector>
#include <map>

namespace MessagingMesh
{
    /// <summary>
    /// Parsed version of mesh-config.json.
    /// </summary>
    struct ParsedMeshConfig
    {
        // Info for one gateway.
        struct GatewayInfo
        {
            // Gateway hostname.
            std::string Hostname;

            // Gateway port.
            int Port;

            // Gateway IP address.
            // NOTE: This is not part of the config. This is looked up from the Hostname.
            std::string IPAddress;  
        };
        using VecGatewayInfo = std::vector<GatewayInfo>;

        // Vector of gateway-infos for each service, keyed by service name.
        std::map<std::string, VecGatewayInfo> Services;
    };
} // namespace


