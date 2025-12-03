#pragma once
#include "GatewayInfo.h"
#include <SharedAliases.h>
#include <Socket.h>

namespace MessagingMesh
{
    // Forward declarations...
    class ServiceManager;

    /// <summary>
    /// Manages a connection to a peer gateway in the mesh.
    /// </summary>
    class MeshGatewayConnection : Socket::ICallback
    {
    // Public methods...
    public:
        // Constructor.
        MeshGatewayConnection(UVLoopPtr pUVLoop, ServiceManager& serviceManager, const GatewayInfo& gatewayInfo);

        // Destructor.
        ~MeshGatewayConnection();

        // Gets the connection status.
        Socket::ConnectionStatus getConnectionStatus() const { return m_connectionStatus; }

        // Relays a message / update to the mesh peer.
        void relay(BufferPtr pBuffer) const;

    // Socket::ICallback implementation
    private:
        // Called when a new client connection has been made to a listening socket.
        void onNewConnection(SocketPtr /*pClientSocket*/) {}

        // Called when data has been received on the socket.
        void onDataReceived(Socket* pSocket, BufferPtr pBuffer);

        // Called when the socket connection status has changed.
        virtual void onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus, const std::string& message);

        // Called when the movement of the socket to a new UV loop has been completed.
        void onMoveToLoopComplete(Socket* /*pSocket*/) {}

    // Private functions...
    private:
        // Connects to the peer gateway.
        void connect();

        // Called when the peer socket connection has succeeded.
        void onConnectionSucceeded();

        // Called when the peer socket connection has failed.
        void onConnectionFailed(const std::string& message);

        // Called when we receive the ACK from a gateway peer.
        void onAck();

    // Private data...
    private:
        // The UV loop we are running in...
        UVLoopPtr m_pUVLoop;

        // The service manager for this mesh...
        ServiceManager& m_serviceManager;

        // Gateway info (hostname, port etc) for the peer gateway we connect to...
        GatewayInfo m_gatewayInfo;

        // The name of the peer (hostname:port)...
        std::string m_peerName;

        // The socket connection to the peer gateway...
        SocketPtr m_pSocket;

        // The status of our connection...
        Socket::ConnectionStatus m_connectionStatus = Socket::ConnectionStatus::WAITING;
    };
} // namespace


