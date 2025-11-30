#pragma once
#include <string>
#include <map>
#include "GatewayInfo.h"

namespace MessagingMesh
{
    // Forward declarations...
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

    // Private functions...
    private:
        // Parses the mesh config from mesh-config.json.
        void parseMeshConfig();

        // Enriches the config, resolving IP addresses and working out which gateways are 'us' and which are peers.
        void enrichConfig();

        // Creates service-managers for meshes to which we belong.
        void createServiceManagers();

    // Private data...
    private:
        // The parent gateway.
        Gateway& m_gateway;

        // Vector of gateway-infos for each service in the mesh, keyed by service name.
        std::map<std::string, VecGatewayInfo> m_gatewayInfos;
    };
} // namespace


