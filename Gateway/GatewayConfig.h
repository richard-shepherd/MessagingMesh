#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "GatewayInfo.h"

namespace MessagingMesh
{
    // Forward declarations...
    class Gateway;

    // Reads gateway-config.json, including enriching the config.
    class GatewayConfig
    {
    // Public types...
    public:
        // Config (enriched) for one mesh in the StartupMeshes section.
        struct StartupMeshConfig
        {
            std::string Name;
            bool DiscoverMeshUsingCoordinator = false;
            std::vector<GatewayInfo> MeshGatewayInfos;
        };

        // Enriched version of gateway-config.json.
        struct Config
        {
            GatewayInfo CoordinatorGateway;
            std::unordered_map<std::string, StartupMeshConfig> StartupMeshConfigs;
        };

    // Public methods...
    public:
        // Constructor.
        GatewayConfig(Gateway& gateway);

        // Parses and enriches gateway-config.json.
        void parse(const std::string& filename);

        // Returns the parsed and enriched config.
        const Config& getConfig() const { return m_config; }

    // Private functions...
    private:
        // Returns a GatewayInfo for the "hostname:port" provided.
        GatewayInfo getGatewayInfo(const std::string& hostnameAndPort);

    // Private data...
    private:
        // The parent gateway.
        Gateway& m_gateway;

        // Config parsed from gateway-config.json
        Config m_config;
    };
} // namespace


