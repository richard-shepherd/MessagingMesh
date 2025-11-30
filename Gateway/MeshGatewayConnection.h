#pragma once
#include "GatewayInfo.h"

namespace MessagingMesh
{


    /// <summary>
    /// Manages a connection to a peer gateway in the mesh.
    /// </summary>
    class MeshGatewayConnection
    {
    // Public methods...
    public:
        // Constructor.
        MeshGatewayConnection(const GatewayInfo& gatewayInfo);

    // Private data...
    private:
        // Gateway info (hostname, port etc) for the peer gateway we connect to...
        const GatewayInfo& m_gatewayInfo;
    };
} // namespace


