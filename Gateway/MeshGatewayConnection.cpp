#include "MeshGatewayConnection.h"
#include <Logger.h>
#include <UVLoop.h>
using namespace MessagingMesh;

// Constructor.
MeshGatewayConnection::MeshGatewayConnection(UVLoopPtr pUVLoop, const GatewayInfo& gatewayInfo) :
    m_pUVLoop(pUVLoop),
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

// Called when the socket connection status has changed.
void MeshGatewayConnection::onConnectionStatusChanged(Socket* /*pSocket*/, Socket::ConnectionStatus connectionStatus, const std::string& message)
{
    try
    {
        switch (connectionStatus)
        {
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            onConnectionSucceeded();
            break;

        case Socket::ConnectionStatus::CONNECTION_FAILED:
            onConnectionFailed(message);
            break;
        }
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
}

// Called when the peer socket connection has failed.
void MeshGatewayConnection::onConnectionFailed(const std::string& message)
{
    Logger::info(std::format("Connection to mesh peer {} failed ({}). Retrying in 30 seconds.", m_peerName, message));

    // We retry after 30 seconds...
    UVUtils::runSingleShotTimer(
        m_pUVLoop->getUVLoop(),
        30000,
        [&]()
        {
            connect();
        }
    );
}


