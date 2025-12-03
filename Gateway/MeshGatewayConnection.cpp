#include "MeshGatewayConnection.h"
#include <Logger.h>
#include <UVLoop.h>
#include <NetworkMessage.h>
#include "ServiceManager.h"
using namespace MessagingMesh;

// Constructor.
MeshGatewayConnection::MeshGatewayConnection(UVLoopPtr pUVLoop, ServiceManager& serviceManager, const GatewayInfo& gatewayInfo) :
    m_pUVLoop(pUVLoop),
    m_serviceManager(serviceManager),
    m_gatewayInfo(gatewayInfo)
{
    m_peerName = std::format("{}:{}", gatewayInfo.Hostname, gatewayInfo.Port);

    // We create the socket to connect to the gateway...
    m_pSocket = Socket::create(m_pUVLoop);
    m_pSocket->setCallback(this);

    // We try to connect...
    connect();
}

// Destructor.
MeshGatewayConnection::~MeshGatewayConnection()
{
}

// Connects to the peer gateway.
void MeshGatewayConnection::connect()
{
    try
    {
        Logger::info(std::format("Trying socket connection for mesh peer: {}", m_peerName));
        m_pSocket->connect(m_gatewayInfo.Hostname, m_gatewayInfo.Port);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when data has been received on the socket.
void MeshGatewayConnection::onDataReceived(Socket* /*pSocket*/, BufferPtr pBuffer)
{
    try
    {
        // The buffer holds a serialized NetworkMessage. We deserialize the header and check the action...
        NetworkMessage networkMessage;
        networkMessage.deserializeHeader(*pBuffer);
        auto& header = networkMessage.getHeader();
        auto action = header.getAction();
        switch (action)
        {
        case NetworkMessageHeader::Action::ACK:
            onAck();
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when the socket connection status has changed.
void MeshGatewayConnection::onConnectionStatusChanged(Socket* /*pSocket*/, Socket::ConnectionStatus connectionStatus, const std::string& message)
{
    try
    {
        // We note the status and act on it...
        m_connectionStatus = connectionStatus;
        switch (connectionStatus)
        {
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            onConnectionSucceeded();
            break;

        case Socket::ConnectionStatus::CONNECTION_FAILED:
            onConnectionFailed(message);
            break;
        }

        // We notify the service-manager that our connection status has changed...
        m_serviceManager.onMeshGatewayConnectionStatusChanged();
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when the peer socket connection has succeeded.
void MeshGatewayConnection::onConnectionSucceeded()
{
    Logger::info(std::format("Connection to mesh peer {} succeeded", m_peerName));

    // We send a CONNECT message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::CONNECT_MESH_PEER);
    header.setSubject(m_serviceManager.getServiceName());
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);
}

// Called when the peer socket connection has failed.
void MeshGatewayConnection::onConnectionFailed(const std::string& message)
{
    Logger::info(std::format("Connection to mesh peer {} failed ({}). Retrying in 30 seconds.", m_peerName, message));

    // We retry after 30 seconds...
    UVUtils::runSingleShotTimer(m_pUVLoop, 30000, [&]() { connect(); });
}

// Called when we receive the ACK from a gateway peer.
void MeshGatewayConnection::onAck()
{
    Logger::info(std::format("Received ACK from mesh peer {}", m_peerName));
}

