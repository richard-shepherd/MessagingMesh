#include "MeshGatewayConnection.h"
#include <Logger.h>
#include <UVLoop.h>
using namespace MessagingMesh;

// Constructor.
MeshGatewayConnection::MeshGatewayConnection(UVLoopPtr pUVLoop, const GatewayInfo& gatewayInfo) :
    m_pUVLoop(pUVLoop),
    m_gatewayInfo(gatewayInfo)
{
    Logger::info(std::format("Creating mesh gateway connection for {}:{}", gatewayInfo.Hostname, gatewayInfo.Port));

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
    m_pSocket->connect(m_gatewayInfo.Hostname, m_gatewayInfo.Port);
}
