#pragma once
#include "ParsedMeshConfig.h"

namespace MessagingMesh
{
    /// <summary>
    /// Helps manage the mesh of gateways.
    /// </summary>
    class MeshManager
    {
    // Public methods...
    public:
        // Constructor.
        MeshManager(int gatewayPort);

        // Destructor.
        ~MeshManager();

    // Private functions...
    private:
        // Parses the mesh config from mesh-config.json.
        void parseMeshConfig();

        // Enriches the config, resolving IP addresses and working out which gateways are 'us' and which are peers.
        void enrichConfig();

    // Private data...
    private:
        // The port on which the hosting gateway is listening.
        // This helps us determine which mesh gateways are 'us' and which are peers.
        int m_gatewayPort;

        // Mesh config...
        ParsedMeshConfig m_meshConfig;
    };
} // namespace


