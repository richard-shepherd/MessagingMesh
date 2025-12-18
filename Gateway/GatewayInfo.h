#pragma once
#include <string>
#include <format>
#include <vector>

namespace MessagingMesh
{
    // Info for one gateway in the mesh.
    struct GatewayInfo
    {
        // Enum for the peer status of a gateway, ie whether it is a peer or 'self' (the active gateway).
        enum class PeerType
        {
            WAITING,
            PEER,
            SELF
        };

        // Gateway hostname.
        std::string Hostname;

        // Gateway port.
        int Port;

        // Gateway IP address.
        // NOTE: This is not part of the config. This is looked up from the Hostname.
        std::string IPAddress;

        // Whether the gateway is a peer or 'self' (ie, the active gateway).
        PeerType PeerType = PeerType::WAITING;

        // Returns a string identifying this gateway-info which can be used as a key to a map.
        std::string makeKey() const
        {
            return std::format("{}~{}", Hostname, Port);
        }
    };

    // A vector of gateway-info.
    using VecGatewayInfo = std::vector<GatewayInfo>;

} // namespace


