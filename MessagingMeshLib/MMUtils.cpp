#include "MMUtils.h"
#include "NetworkMessage.h"
#include "Socket.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Sends a network-message to the socket.
void MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket)
{
    sendNetworkMessage(networkMessage, pSocket.get());
}

// Sends a network-message to the socket.
void MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket)
{
    // We serialize the message and send it...
    auto pBuffer = Buffer::create();
    networkMessage.serialize(*pBuffer);
    pSocket->write(pBuffer);
}
