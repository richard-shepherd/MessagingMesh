#include "Gateway.h"
#include "Socket.h"
#include "Utils.h"
#include "Logger.h"
#include "NetworkMessage.h"
#include "Message.h"
#include "Field.h"
#include "Exception.h"
using namespace MessagingMesh;

// Constructor.
Gateway::Gateway(int port) :
    m_port(port),
    m_pUVLoop(UVLoop::create("GATEWAY"))
{
    // We create a socket to listen to client connections, managed by the uv loop...
    m_pUVLoop->marshallEvent(
        [this](uv_loop_t* /*pLoop*/)
        {
            createListeningSocket();
        }
    );
}

// Creates the socket to listen for client connections.
void Gateway::createListeningSocket()
{
    try
    {
        m_listeningSocket = Socket::create(m_pUVLoop);
        m_listeningSocket->setCallback(this);
        m_listeningSocket->listen(m_port);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
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
        m_pendingConnections[pSocket->getName()] = pSocket;
        pSocket->setCallback(this);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
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
            onConnect(pSocket->getName(), header);
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when a socket has been disconnected.
// Called on the socket's thread.
void Gateway::onDisconnected(Socket* pSocket)
{
    try
    {
        // We do not usually expect to get a socket disconnection here.
        // In most cases a client socket will have sent the CONNECT message and been
        // moved to be managed by a ServiceManager. We can get the disconnection here
        // if the socket has disconnected more-or-less immediately after the original
        // connection, before we get the CONNECT message.
        // 
        // If this happens, we remove the socket from the pending-collection. This 
        // releases our reference to it, allowing it to be destructed.
        auto& socketName = pSocket->getName();
        m_pendingConnections.erase(socketName);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when we receive a CONNECT message from a client.
void Gateway::onConnect(const std::string& socketName, const NetworkMessageHeader& header)
{
    // We log the connect request...
    auto& service = header.getSubject();
    Logger::info(Utils::format("Received CONNECT request from %s for service %s", socketName.c_str(), service.c_str()));

    // We find the socket from the pending-collection...
    auto it_pendingConnections = m_pendingConnections.find(socketName);
    if (it_pendingConnections == m_pendingConnections.end())
    {
        auto message = Utils::format("Socket %s not in pending-collection", socketName.c_str());
        throw Exception(message);
    }
    auto pSocket = it_pendingConnections->second;

    // We get or create the ServiceManager for the service requested by the client...
    auto it_serviceManagers = m_serviceManagers.find(service);
    if (it_serviceManagers == m_serviceManagers.end())
    {
        it_serviceManagers = m_serviceManagers.insert(it_serviceManagers, { service, ServiceManager(service) });
    }
    auto& serviceManager = it_serviceManagers->second;
    
    // We move the socket to the service-manager...
    serviceManager.registerSocket(pSocket);

    // The socket is now managed by the service-manager, so we remove it from our pending-collection...
    m_pendingConnections.erase(socketName);
}
