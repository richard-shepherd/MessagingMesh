#pragma once
#include "ParsedMeshConfig.h"

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

        // Mesh config...
        ParsedMeshConfig m_meshConfig;
    };
} // namespace


