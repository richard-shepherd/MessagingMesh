#include "ServiceManager.h"
#include <format>
#include <UVLoop.h>
#include <Socket.h>
#include <Logger.h>
#include <MMUtils.h>
#include <Utils.h>
#include <NetworkMessage.h>
#include "SubscriptionInfo.h"
using namespace MessagingMesh;

// Constructor.
ServiceManager::ServiceManager(const std::string& serviceName) :
    m_serviceName(serviceName),
    m_pUVLoop(UVLoop::create(serviceName))
{
    Logger::info(std::format("Created ServiceManager for {}", serviceName));
}

// Registers a client socket to be managed for this service.
void ServiceManager::registerSocket(SocketPtr pSocket)
{
    // We add the socket to the collection of active clients...
    m_clientSockets[pSocket->getName()] = pSocket;

    // We observe updates from the socket...
    pSocket->setCallback(this);

    // We move the socket to our UV loop...
    pSocket->moveToLoop(m_pUVLoop);
}

// Called when data has been received on the socket.
// Called on the thread of the client socket.
void ServiceManager::onDataReceived(Socket* pSocket, BufferPtr pBuffer)
{
    try
    {
        // The buffer holds a serialized NetworkMessage. We deserialize the header...
        NetworkMessage networkMessage;
        networkMessage.deserializeHeader(*pBuffer);
        auto& header = networkMessage.getHeader();
        auto action = header.getAction();

        // We process the update depending on the action...
        switch (action)
        {
        case NetworkMessageHeader::Action::SUBSCRIBE:
            onSubscribe(pSocket, header);
            break;

        case NetworkMessageHeader::Action::UNSUBSCRIBE:
            onUnsubscribe(pSocket, header);
            break;

        case NetworkMessageHeader::Action::SEND_MESSAGE:
            onMessage(header, pBuffer);
            break;

        case NetworkMessageHeader::Action::DISCONNECT:
            onDisconnected(pSocket);
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when a socket has been disconnected.
// Called on the socket's thread.
void ServiceManager::onDisconnected(Socket* pSocket)
{
    try
    {
        // We remove any active subscriptions for the client...
        auto& socketName = pSocket->getName();
        m_subjectMatchingEngine.removeAllSubscriptions(socketName);

        // We remove the socket from the collection of client sockets...
        m_clientSockets.erase(socketName);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when the movement of the socket to a new UV loop has been completed.
void ServiceManager::onMoveToLoopComplete(Socket* pSocket)
{
    try
    {
        // We send an ACK message to the client to let them know that the
        // CONNECT has completed successfully...
        NetworkMessage connectMessage;
        auto& header = connectMessage.getHeader();
        header.setAction(NetworkMessageHeader::Action::ACK);
        MMUtils::sendNetworkMessage(connectMessage, pSocket);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when we receive a SUBSCRIBE message.
void ServiceManager::onSubscribe(Socket* pSocket, const NetworkMessageHeader& header)
{
    // We register the subscription with the subject matching engine...
    m_subjectMatchingEngine.addSubscription(
        header.getSubject(),
        header.getSubscriptionID(),
        pSocket->getName(),
        pSocket);
}

// Called when we receive an UNSUBSCRIBE message.
void ServiceManager::onUnsubscribe(Socket* pSocket, const NetworkMessageHeader& header)
{
    // We unregister the subscription from the subject matching engine...
    m_subjectMatchingEngine.removeSubscription(
        header.getSubject(),
        pSocket->getName());
}

// Called when we receive a SEND_MESSAGE message.
void ServiceManager::onMessage(const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We find the clients which have subscriptions to the message subject...
    auto subscriptionInfos = m_subjectMatchingEngine.getMatchingSubscriptionInfos(header.getSubject());

    // We send the update to each client...
    for (const auto& pSubscriptionInfo : subscriptionInfos)
    {
        pSubscriptionInfo->getSocket()->write(pBuffer, pSubscriptionInfo->getSubscriptionID());
    }
}


