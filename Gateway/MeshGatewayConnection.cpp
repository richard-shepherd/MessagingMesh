#include "MeshGatewayConnection.h"
#include <Logger.h>
using namespace MessagingMesh;

// Constructor.
MeshGatewayConnection::MeshGatewayConnection(const GatewayInfo& gatewayInfo) :
    m_gatewayInfo(gatewayInfo)
{
    Logger::info(std::format("Creating mesh gateway connection for {}:{}", gatewayInfo.Hostname, gatewayInfo.Port));
}
