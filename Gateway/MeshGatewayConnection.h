#pragma once
#include "GatewayInfo.h"
#include <SharedAliases.h>
#include <Socket.h>

namespace MessagingMesh
{
    /// <summary>
    /// Manages a connection to a peer gateway in the mesh.
    /// </summary>
    class MeshGatewayConnection : Socket::ICallback
    {
    // Public methods...
    public:
        // Constructor.
        MeshGatewayConnection(UVLoopPtr pUVLoop, const GatewayInfo& gatewayInfo);

        // Destructor.
        ~MeshGatewayConnection();

    // Socket::ICallback implementation
    private:
        // Called when a new client connection has been made to a listening socket.
        void onNewConnection(SocketPtr /*pClientSocket*/) {}

        // Called when data has been received on the socket.
        void onDataReceived(Socket* /*pSocket*/, BufferPtr /*pBuffer*/) {}

        // Called when the socket connection status has changed.
        virtual void onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus);

        // Called when the movement of the socket to a new UV loop has been completed.
        void onMoveToLoopComplete(Socket* /*pSocket*/) {}

    // Private functions...
    private:
        // Connects to the peer gateway.
        void connect();

    // Private data...
    private:
        // The UV loop we are running in...
        UVLoopPtr m_pUVLoop;

        // Gateway info (hostname, port etc) for the peer gateway we connect to...
        const GatewayInfo& m_gatewayInfo;

        // The socket connection to the peer gateway...
        SocketPtr m_pSocket;
    };
} // namespace


