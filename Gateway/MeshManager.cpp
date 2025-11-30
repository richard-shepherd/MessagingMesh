#include "MeshManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <MMUtils.h>
#include <Logger.h>
#include "Gateway.h"
using namespace MessagingMesh;
using json = nlohmann::json;

// Constructor.
MeshManager::MeshManager(Gateway& gateway) :
    m_gateway(gateway)
{
    // We parse and enrich the mesh config...
    parseMeshConfig();
    enrichConfig();

    // We pre-create service managers for meshes to which we belong...
}

// Destructor.
MeshManager::~MeshManager()
{
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
    Logger::info("Enriching mesh config");

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

}

