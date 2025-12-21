#include "Gateway.h"
#include <format>
#include <Socket.h>
#include <Logger.h>
#include <NetworkMessage.h>
#include <Exception.h>
#include "MeshManager.h"
using namespace MessagingMesh;

// Constructor.
Gateway::Gateway(int port) :
    m_port(port),
    m_pUVLoop(UVLoop::create("GATEWAY", UVLoop::Temperature::COLD)),
    m_meshManager(*this)
{
    // We initialize the gateway in the context of the UV loop...
    m_pUVLoop->marshallEvent(
        [this](uv_loop_t* /*pLoop*/)
        {
            initialize();
        }
    );
}

// Destructor.
Gateway::~Gateway()
{
}

// Initializes the gateway, including creating the socket to listen for client connections.
void Gateway::initialize()
{
    try
    {
        // Find our hostname and IP address...
        m_hostname = MMUtils::getHostname();
        m_ipAddress = MMUtils::getIPAddress();

        // We create the socket to listen for client connections...
        m_listeningSocket = Socket::create(m_pUVLoop);
        m_listeningSocket->setCallback(this);
        m_listeningSocket->listen(m_port);

        // We initialize the mesh manager.
        // NOTE: We need to do this here, as we want UV (and the socket library) to be ready.
        m_meshManager.initialize();
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when a new client connection has been made to a listening socket.
// Called on the GATEWAY thread.
void Gateway::onNewConnection(SocketPtr pSocket)
{
    try
    {
        // We add the socket to the pending-collection and observe it 
        // to listen for the CONNECT message...
        m_pendingConnections[pSocket->getSocketID()] = pSocket;
        pSocket->setCallback(this);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when data has been received on the socket.
// Called on the thread of the client socket.
void Gateway::onDataReceived(Socket* pSocket, BufferPtr pBuffer)
{
    try
    {
        // The buffer holds a serialized NetworkMessage. We deserialize
        // the header and check the action...
        NetworkMessage networkMessage;
        networkMessage.deserializeHeader(*pBuffer);
        auto& header = networkMessage.getHeader();
        auto action = header.getAction();
        switch (action)
        {
        case NetworkMessageHeader::Action::CONNECT:
        case NetworkMessageHeader::Action::CONNECT_MESH_PEER:
            onConnect(pSocket->getSocketID(), header);
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when the connection status has changed.
// Called on the socket's thread.
void Gateway::onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus, const std::string& /*message*/)
{
    try
    {
        if (connectionStatus == Socket::ConnectionStatus::DISCONNECTED)
        {
            // We do not usually expect to get a socket disconnection here.
            // In most cases a client socket will have sent the CONNECT message and been
            // moved to be managed by a ServiceManager. We can get the disconnection here
            // if the socket has disconnected more-or-less immediately after the original
            // connection, before we get the CONNECT message.
            // 
            // If this happens, we remove the socket from the pending-collection. This 
            // releases our reference to it, allowing it to be destructed.
            auto socketID = pSocket->getSocketID();
            m_pendingConnections.erase(socketID);
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when we receive a CONNECT message from a client.
void Gateway::onConnect(uint64_t socketID, const NetworkMessageHeader& header)
{
    // We log the connect request...
    auto& service = header.getSubject();
    bool isMeshPeer = false;
    std::string strAction;
    switch (header.getAction())
    {
    case NetworkMessageHeader::Action::CONNECT:
        isMeshPeer = false;
        strAction = "CONNECT";
        break;

    case NetworkMessageHeader::Action::CONNECT_MESH_PEER:
        isMeshPeer = true;
        strAction = "CONNECT_MESH_PEER";
        break;
    }
    // We find the socket from the pending-collection...
    auto it_pendingConnections = m_pendingConnections.find(socketID);
    if (it_pendingConnections == m_pendingConnections.end())
    {
        auto message = std::format("Socket {} not in pending-collection", socketID);
        throw Exception(message);
    }
    auto pSocket = it_pendingConnections->second;

    // We update the socket name to include the Client ID (which is in the reply-subject for a CONNECT message)...
    pSocket->setClientID(header.getReplySubject());
    Logger::info(std::format("Received {} request from {} for service {}", strAction, pSocket->getName(), service));

    // We get or create the ServiceManager for the service requested by the client...
    auto& serviceManager = getOrCreateServiceManager(service);
    
    // We move the socket to the service-manager...
    serviceManager.registerSocket(pSocket, isMeshPeer);

    // The socket is now managed by the service-manager, so we remove it from our pending-collection...
    m_pendingConnections.erase(socketID);
}

// Gets or creates a service-manager for the specified service.
ServiceManager& Gateway::getOrCreateServiceManager(const std::string& service)
{
    auto [it, inserted] = m_serviceManagers.try_emplace(service, service, *this, m_meshManager);
    return it->second;
}
