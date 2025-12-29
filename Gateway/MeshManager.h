#pragma once
#include <string>
#include "Callbacks.h"
#include "GatewayInfo.h"
#include "GatewayConfig.h"

namespace MessagingMesh
{
    // Forward declarations...
    class Connection;
    class Gateway;

    /// <summary>
    /// Helps manage the mesh of gateways.
    /// </summary>
    class MeshManager
    {
    // Public methods...
    public:
        // Constructor.
        MeshManager(Gateway& gateway);

        // Destructor.
        ~MeshManager();

        // Initializes the mesh-manager.
        // NOTE: Initialization is not done in the constructor, as it needs to be done at a later point when the
        //       parent Gateway's UV loop is running.
        void initialize();

        // Returns a vector of gateway-info for peer-gateways in the mesh for the service-name specified.
        VecGatewayInfo getPeerGatewayInfos(const std::string& serviceName) const;

        // Sends a message to the coordinator.
        void sendMessageToCoordinator(const MessagePtr& pMessage, const std::string& subject) const;

    // Private functions...
    private:
        // Creates the connection to the Coordinator.
        void createCoordinatorConnection();

        // Creates service-managers for meshes to which we belong.
        void createServiceManagers();

        // Called when we get a notification from the Coordinator connection.
        void onCoordinatorConnectionNotification(Connection& connection, NotificationType notificationType, const std::string& info);

    // Private data...
    private:
        // The parent gateway.
        Gateway& m_gateway;

        // Parsed and enriched gateway-config.json.
        GatewayConfig m_gatewayConfig;

        // Connection to the Coordinator.
        Connection* m_pCoordinatorConnection = nullptr;
        bool m_coordinatorConnected = false;
    };
} // namespace


