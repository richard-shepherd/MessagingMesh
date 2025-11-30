#include "MeshManager.h"
#include <fstream>
#include <ranges>
#include <nlohmann/json.hpp>
#include <MMUtils.h>
#include <Logger.h>
#include "Gateway.h"
using namespace MessagingMesh;
using json = nlohmann::json;

// Constructor.
MeshManager::MeshManager(Gateway& gateway) :
    m_gateway(gateway)
{
}

// Destructor.
MeshManager::~MeshManager()
{
}

// Initializes the mesh-manager.
// NOTE: Initialization is not done in the constructor, as it needs to be done at a later point when the
//       parent Gateway's UV loop is running.
void MeshManager::initialize()
{
    // We parse and enrich the mesh config...
    parseMeshConfig();
    enrichConfig();

    // We pre-create service managers for meshes to which we belong...
    createServiceManagers();
}

// Returns a vector of gateway-info for peer-gateways in the mesh for the service-name specified.
ParsedMeshConfig::VecGatewayInfo MeshManager::getPeerGatewayInfos(const std::string& serviceName) const
{
    ParsedMeshConfig::VecGatewayInfo result;

    // We find the gateway-infos for the service...
    auto it = m_meshConfig.Services.find(serviceName);
    if (it == m_meshConfig.Services.end())
    {
        // We do not know about this service, so we return an empty collection...
        return result;
    }

    // We have the service, so we look through the gateway-infos, returning all that are peers...
    const auto& gatewayInfos = it->second;
    for (const auto& gatewayInfo : gatewayInfos)
    {
        if (gatewayInfo.PeerType == ParsedMeshConfig::PeerType::PEER)
        {
            result.push_back(gatewayInfo);
        }
    }
    return result;
}

// Parses the mesh config from mesh-config.json.
void MeshManager::parseMeshConfig()
{
    Logger::info("Parsing mesh config");

    // We read the file and parse the JSON...
    std::ifstream file("mesh-config.json");
    json j = json::parse(file);

    // We read the JSON into the parsed config structure...
    for (auto& [serviceName, jsonGatewayInfos] : j.items()) 
    {
        // We parse each gateway-info...
        ParsedMeshConfig::VecGatewayInfo gatewayInfos;
        for (auto& jsonGatewayInfo : jsonGatewayInfos) 
        {
            ParsedMeshConfig::GatewayInfo gatewayInfo;
            gatewayInfo.Hostname = jsonGatewayInfo["Hostname"].get<std::string>();
            gatewayInfo.Port = jsonGatewayInfo["Port"].get<int>();
            gatewayInfos.push_back(gatewayInfo);
        }
        m_meshConfig.Services[serviceName] = gatewayInfos;
    }
}

// Enriches the config, resolving IP addresses and working out which gateways are 'us' and which are peers.
void MeshManager::enrichConfig()
{
    Logger::info("Enriching mesh config (resolving peer IP addresses)");

    // We find the IP address of the running process. This helps us check which gateways are 'us'...
    auto gatewayIPAddress = MMUtils::getIPAddress();

    // We iterate through the services in the config...
    for (auto& [serviceName, gatewayInfos] : m_meshConfig.Services)
    {
        // We iterate through the gateway-infos for the service, resolving each to its
        // IP address and checking if it is "us" or not...
        for (auto& gatewayInfo : gatewayInfos)
        {
            // We resolve the IP address...
            gatewayInfo.IPAddress = MMUtils::getIPAddress(gatewayInfo.Hostname);
            if (gatewayInfo.IPAddress.empty())
            {
                Logger::info(std::format("Failed to resolve IP address for Service={}, Hostname={}", serviceName, gatewayInfo.Hostname));
                continue;
            }

            // We check whether this entry is 'us' or a peer...
            if (gatewayInfo.IPAddress == gatewayIPAddress && gatewayInfo.Port == m_gateway.getPort())
            {
                gatewayInfo.PeerType = ParsedMeshConfig::PeerType::SELF;
            }
            else
            {
                gatewayInfo.PeerType = ParsedMeshConfig::PeerType::PEER;
            }
        }
    }
}

// Creates service-managers for meshes to which we belong.
void MeshManager::createServiceManagers()
{
    // We iterate through the services in the config...
    for (auto& [serviceName, gatewayInfos] : m_meshConfig.Services)
    {
        // We check if any of the gateways for the service is SELF...
        if (std::ranges::any_of(gatewayInfos, [](const auto& g) {return g.PeerType == ParsedMeshConfig::PeerType::SELF;}))
        {
            // We are a member of the mesh for this service so we create the service manager...
            Logger::info(std::format("We are a member of the {} mesh", serviceName));
            m_gateway.getOrCreateServiceManager(serviceName);
        }
    }
}

