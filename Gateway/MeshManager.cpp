#include "MeshManager.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <Connection.h>
#include <ConnectionParams.h>
#include <MMUtils.h>
#include <Logger.h>
#include "Gateway.h"
using namespace MessagingMesh;
using json = nlohmann::json;

// Constructor.
MeshManager::MeshManager(Gateway& gateway) :
    m_gateway(gateway),
    m_gatewayConfig(gateway)
{
}

// Destructor.
MeshManager::~MeshManager()
{
    // RSSTODO: PUT THIS BACK! (AND MAKE SURE THE SHUTODWN WORKS CORRECTLY.)
    //delete m_pCoordinatorConnection;
}

// Initializes the mesh-manager.
// NOTE: Initialization is not done in the constructor, as it needs to be done at a later point when the
//       parent Gateway's UV loop is running.
void MeshManager::initialize()
{
    // We parse and enrich the mesh config...
    m_gatewayConfig.parse("gateway-config.json");

    // We create the connection to the Coordinator (if it was specified in the config)...
    createCoordinatorConnection();

    // We pre-create service managers for meshes to which we belong...
    createServiceManagers();
}

// Returns a vector of gateway-info for peer-gateways in the mesh for the service-name specified.
VecGatewayInfo MeshManager::getPeerGatewayInfos(const std::string& serviceName) const
{
    VecGatewayInfo result;

    // We find the gateway-infos for the service...
    const auto& config = m_gatewayConfig.getConfig();
    const auto& startupMeshConfigs = config.StartupMeshConfigs;
    auto it = startupMeshConfigs.find(serviceName);
    if (it == startupMeshConfigs.end())
    {
        // We do not know about this service, so we return an empty collection...
        return result;
    }

    // We have the service, so we look through the gateway-infos, returning all that are peers...
    const auto& startupMeshConfig = it->second;
    const auto& gatewayInfos = startupMeshConfig.MeshGatewayInfos;
    for (const auto& gatewayInfo : gatewayInfos)
    {
        if (gatewayInfo.PeerType == GatewayInfo::PeerType::PEER)
        {
            result.push_back(gatewayInfo);
        }
    }
    return result;
}

// Sends a message to the coordinator.
void MeshManager::sendMessageToCoordinator(const MessagePtr& pMessage, const std::string& subject) const
{
    if (m_coordinatorConnected)
    {
        m_pCoordinatorConnection->sendMessage(pMessage, subject);
    }
}

// Creates the connection to the Coordinator.
void MeshManager::createCoordinatorConnection()
{
    // We check if the Coordinator was specified in the config...
    auto& config = m_gatewayConfig.getConfig();
    auto& gatewayInfo = config.CoordinatorGateway;
    if (gatewayInfo.Hostname.empty())
    {
        return;
    }

    // The Coordinator gateway was specified, so we connect to it...
    ConnectionParams connectionParams;
    connectionParams.GatewayHost = gatewayInfo.Hostname;
    connectionParams.GatewayPort = gatewayInfo.Port;
    connectionParams.Service = "_COORDINATOR";
    connectionParams.MessageDispatch = ConnectionParams::MessageDispatch::CALLBACK_ON_MESSAGING_MESH_THREAD;
    connectionParams.ConnectAsynchronously = true;
    connectionParams.NotificationCallback = [this](Connection& connection, NotificationType notificationType, const std::string& info) 
        {
            onCoordinatorConnectionNotification(connection, notificationType, info);
        };
    m_pCoordinatorConnection = new Connection(connectionParams);
}

// Called when we get a notification from the Coordinator connection.
void MeshManager::onCoordinatorConnectionNotification(Connection& /*connection*/, NotificationType notificationType, const std::string& /*info*/)
{
    switch (notificationType)
    {
    case NotificationType::CONNECTED:
        m_coordinatorConnected = true;
        Logger::info("Coordinator connected");
        break;
    }
}

// Creates service-managers for meshes to which we belong.
void MeshManager::createServiceManagers()
{
    // We iterate through the services in the config...
    const auto& config = m_gatewayConfig.getConfig();
    for (auto& [serviceName, startupMeshConfig] : config.StartupMeshConfigs)
    {
        // We check if any of the gateways for the service is SELF...
        if (std::ranges::any_of(startupMeshConfig.MeshGatewayInfos, [](const auto& g) {return g.PeerType == GatewayInfo::PeerType::SELF;}))
        {
            // We are a member of the mesh for this service so we create the service manager...
            Logger::info(std::format("We are a member of the {} mesh", serviceName));
            m_gateway.getOrCreateServiceManager(serviceName);
        }
    }
}

