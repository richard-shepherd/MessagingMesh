#include "GatewayConfig.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <MMUtils.h>
#include <Exception.h>
#include "Gateway.h"
using namespace MessagingMesh;

// Raw config for one mesh in the StartupMeshes section, and a JSON parsing helper for it.
struct RawStartupMeshConfig
{
    std::string Name;
    bool DiscoverMeshUsingCoordinator = false;
    std::vector<std::string> MeshGateways;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
(
    RawStartupMeshConfig,
    Name,
    DiscoverMeshUsingCoordinator,
    MeshGateways
)

// Raw config parsed from gateway-config.json, and a JSON parsing helper for it.
struct RawConfig
{
    std::string CoordinatorGateway;
    std::vector<RawStartupMeshConfig> StartupMeshes;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
(
    RawConfig,
    CoordinatorGateway,
    StartupMeshes
)

// Constructor.
GatewayConfig::GatewayConfig(Gateway& gateway) :
    m_gateway(gateway)
{
}

// Parses and enriches gateway-config.json.
void GatewayConfig::parse(const std::string& filename)
{
    // We parse the raw config...
    std::ifstream file(filename);
    nlohmann::json j = nlohmann::json::parse(file, nullptr, true, true);
    auto rawConfig = j.get<RawConfig>();

    // We enrich the config.

    // Coordinator gateway...
    if (!rawConfig.CoordinatorGateway.empty())
    {
        m_config.CoordinatorGateway = getGatewayInfo(rawConfig.CoordinatorGateway);
    }

    // Startup meshes...
    for (const auto& rawStartupMeshConfig : rawConfig.StartupMeshes)
    {
        StartupMeshConfig meshConfig;
        meshConfig.Name = rawStartupMeshConfig.Name;
        meshConfig.DiscoverMeshUsingCoordinator = rawStartupMeshConfig.DiscoverMeshUsingCoordinator;
        for (const auto& meshGateway : rawStartupMeshConfig.MeshGateways)
        {
            meshConfig.MeshGatewayInfos.push_back(getGatewayInfo(meshGateway));
        }
        m_config.StartupMeshConfigs[meshConfig.Name] = meshConfig;
    }
}

// Returns a GatewayInfo for the "hostname:port" provided.
GatewayInfo GatewayConfig::getGatewayInfo(const std::string& hostnameAndPort)
{
    GatewayInfo gatewayInfo;

    // We parse the "hostname:port" string...
    auto tokens = MMUtils::tokenize(hostnameAndPort, ':');
    if (tokens.size() != 2)
    {
        throw Exception(std::format("{} is not a valid 'hostname:port'", hostnameAndPort));
    }
    gatewayInfo.Hostname = tokens[0];
    auto strPort = std::string(tokens[1]);
    gatewayInfo.Port = std::stoi(strPort);

    // We find the IP address...
    gatewayInfo.IPAddress = MMUtils::getIPAddress(gatewayInfo.Hostname);
    if (gatewayInfo.IPAddress.empty())
    {
        throw Exception(std::format("Failed to resolve IP address for Hostname={}", gatewayInfo.Hostname));
    }

    // We check whether this entry is 'us' or a peer...
    if (gatewayInfo.IPAddress == m_gateway.getIPAddress() && gatewayInfo.Port == m_gateway.getPort())
    {
        gatewayInfo.PeerType = GatewayInfo::PeerType::SELF;
    }
    else
    {
        gatewayInfo.PeerType = GatewayInfo::PeerType::PEER;
    }

    return gatewayInfo;
}



