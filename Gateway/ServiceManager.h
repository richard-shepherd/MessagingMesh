#pragma once
#include <unordered_map>
#include <string>
#include <SharedAliases.h>
#include <Socket.h>
#include "SubjectMatchingEngine.h"
#include "MeshGatewayConnection.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessageHeader;
    class MeshManager;

    /// <summary>
    /// Manages a messaging-mesh service. 
    /// 
    /// A service is a 'namespace' of messaging. Clients connecting to a service
    /// can send messages to each other, with message sent subjects only matching
    /// to subscriptions to the same service.
    /// 
    /// UV loop and thread
    /// ------------------
    /// Each service runs it own UV loop, so message processing for each service is
    /// managed on its own thread. As all updates on the UV loop take place on the
    /// (single) UV loop thread, this means that we do not have to lock service
    /// specific code such as the subject-matching engine.
    /// </summary>
    class ServiceManager : public Socket::ICallback
    {
    // Public methods...
    public:
        // Constructor.
        ServiceManager(const std::string& serviceName, const MeshManager& meshManager);

        // Destructor.
        ~ServiceManager();

        // Registers a client socket to be managed for this service.
        void registerSocket(SocketPtr pSocket, bool isMeshPeer);

        // Gets the service name.
        const std::string& getServiceName() const { return m_serviceName; }

        // Called when the connection status has changed for a mesh gateway connection.
        void onMeshGatewayConnectionStatusChanged();

        // Called when we receive a SEND_MESSAGE message.
        void onMessage(const NetworkMessageHeader& header, Socket* pSocket, BufferPtr pBuffer);

    // Socket::ICallback implementation...
    private:
        // Called when a new client connection has been made to a listening socket.
        // Called on the GATEWAY thread.
        void onNewConnection(SocketPtr /*pClientSocket*/) {}

        // Called when data has been received on the socket.
        void onDataReceived(Socket* pSocket, BufferPtr pBuffer);

        // Called when the connection status has changed.
        virtual void onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus, const std::string& message);

        // Called when the movement of the socket to a new UV loop has been completed.
        void onMoveToLoopComplete(Socket* pSocket);

    // Private functions...
    private:
        // Initializes the service manager in the context of the service's UV loop.
        void initialize();

        // Called when we receive a SUBSCRIBE message.
        void onSubscribe(Socket* pSocket, const NetworkMessageHeader& header, BufferPtr pBuffer);

        // Called when we receive an UNSUBSCRIBE message.
        void onUnsubscribe(Socket* pSocket, const NetworkMessageHeader& header, BufferPtr pBuffer);

        // Called when a socket has been disconnected.
        void onDisconnected(Socket* pSocket);

        // Relays the message / update in the buffer to all mesh peers.
        void relayToMesh(BufferPtr pBuffer);

    // Private data...
    private:
        // The service name...
        std::string m_serviceName;

        // Tells us about peer gateways in the mesh...
        const MeshManager& m_meshManager;

        // UV loop for processing client messages...
        UVLoopPtr m_pUVLoop;

        // Client sockets, keyed by socket ID...
        std::unordered_map<uint64_t, SocketPtr> m_clientSockets;

        // Maps sent messages to clients who are subscribed to them...
        SubjectMatchingEngine m_subjectMatchingEngine;

        // Peer gateways in the mesh, keyed by GatewayInfo.makeKey().
        // These are the connections where we act as the client to the peer gateway.
        std::map<std::string, MeshGatewayConnection> m_meshGatewayConnections_WeAreTheClient;

        // Sockets for peer gateways in the mesh, keyed by socket ID.
        // These are the connections where we act as the server to the peer gateway.
        std::unordered_map<uint64_t, SocketPtr> m_meshGatewayConnections_WeAreTheServer;
    };
} // namespace

