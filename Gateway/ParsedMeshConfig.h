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
        // Enum for the peer status of a gateway, ie whether it is a peer or 'self' (the active gateway).
        enum class PeerType
        {
            WAITING,
            PEER,
            SELF
        };

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

            // Whether the gateway is a peer or 'self' (ie, the active gateway).
            PeerType PeerType = PeerType::WAITING;
        };
        using VecGatewayInfo = std::vector<GatewayInfo>;

        // Vector of gateway-infos for each service, keyed by service name.
        std::map<std::string, VecGatewayInfo> Services;
    };
} // namespace


