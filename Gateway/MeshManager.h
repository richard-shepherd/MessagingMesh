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
        MeshManager();

        // Destructor.
        ~MeshManager();

    // Private functions...
    private:
        // Parses the mesh config from mesh-config.json.
        void parseMeshConfig();

    // Private data...
    private:
        // Mesh config...
        ParsedMeshConfig m_meshConfig;
    };
} // namespace


