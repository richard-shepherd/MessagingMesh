#include "ServiceManager.h"
#include <algorithm>
#include <format>
#include <unordered_set>
#include <UVLoop.h>
#include <Socket.h>
#include <Logger.h>
#include <MMUtils.h>
#include <Utils.h>
#include <NetworkMessage.h>
#include "SubscriptionInfo.h"
#include "MeshManager.h"
using namespace MessagingMesh;

// Constructor.
ServiceManager::ServiceManager(const std::string& serviceName, const MeshManager& meshManager) :
    m_serviceName(serviceName),
    m_meshManager(meshManager),
    m_pUVLoop(UVLoop::create(serviceName))
{
    // We initialize the service manager in the context of the UV loop...
    m_pUVLoop->marshallEvent(
        [this](uv_loop_t* /*pLoop*/)
        {
            initialize();
        }
    );
}

// Destructor.
ServiceManager::~ServiceManager()
{
}

// Registers a client socket to be managed for this service.
void ServiceManager::registerSocket(SocketPtr pSocket, bool isMeshPeer)
{
    // We add the socket to the collection of active clients...
    if (isMeshPeer)
    {
        // This is a mesh peer...
        m_meshGatewayConnections_WeAreTheServer[pSocket->getName()] = pSocket;
    }
    else
    {
        // This is a client connection...
        m_clientSockets[pSocket->getName()] = pSocket;
    }

    // We note whether the socket is a mesh peer (ie, a gateway in the mesh).
    pSocket->setIsMeshPeer(isMeshPeer);

    // We observe updates from the socket...
    pSocket->setCallback(this);

    // We move the socket to our UV loop...
    pSocket->moveToLoop(m_pUVLoop);
}

// Called when the connection status has changed for a mesh gateway connection.
void ServiceManager::onMeshGatewayConnectionStatusChanged()
{
    // We check if we are connected to all gateways in the mesh...
    auto allConnected = std::ranges::all_of(
        m_meshGatewayConnections_WeAreTheClient,
        [](const auto& pair) {return pair.second.getConnectionStatus() == Socket::ConnectionStatus::CONNECTION_SUCCEEDED;});
    if (allConnected)
    {
        Logger::info("All mesh gateways connected");
    }
}

// Initializes the service manager in the context of the service's UV loop.
void ServiceManager::initialize()
{
    Logger::info(std::format("Initializing ServiceManager for {}", m_serviceName));

    // We create mesh gateway connections to each peer gateway in the mesh...
    auto peerGatewayInfos = m_meshManager.getPeerGatewayInfos(m_serviceName);
    for (const auto& peerGatewayInfo : peerGatewayInfos)
    {
        auto key = peerGatewayInfo.makeKey();
        m_meshGatewayConnections_WeAreTheClient.try_emplace(key, m_pUVLoop, *this, peerGatewayInfo);
    }
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
            onSubscribe(pSocket, header, pBuffer);
            break;

        case NetworkMessageHeader::Action::UNSUBSCRIBE:
            onUnsubscribe(pSocket, header, pBuffer);
            break;

        case NetworkMessageHeader::Action::SEND_MESSAGE:
            onMessage(header, pSocket, pBuffer);
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


// Called when the connection status has changed.
void ServiceManager::onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus, const std::string& /*message*/)
{
    try
    {
        switch (connectionStatus)
        {
        case Socket::ConnectionStatus::DISCONNECTED:
            onDisconnected(pSocket);
            break;
        }
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

// Called when a socket has been disconnected.
void ServiceManager::onDisconnected(Socket* pSocket)
{
    try
    {
        // We remove any active subscriptions for the client...
        auto& socketName = pSocket->getName();
        m_subjectMatchingEngine.removeAllSubscriptions(socketName);

        // We remove the socket from the collections of sockets we manage...
        m_clientSockets.erase(socketName);
        m_meshGatewayConnections_WeAreTheServer.erase(socketName);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when we receive a SUBSCRIBE message.
void ServiceManager::onSubscribe(Socket* pSocket, const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We register the subscription with the subject matching engine...
    auto subscriptionCount = m_subjectMatchingEngine.addSubscription(
        header.getSubject(),
        header.getSubscriptionID(),
        pSocket->getName(),
        pSocket);

    // If this is the first subscription we have for this subject AND the subscription came
    // from a client (not a mesh peer) then we forward the subscription to the mesh...
    if (subscriptionCount == 1 && pSocket->getIsMeshPeer() == false)
    {
        relayToMesh(pBuffer);
    }
}

// Called when we receive an UNSUBSCRIBE message.
void ServiceManager::onUnsubscribe(Socket* pSocket, const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We unregister the subscription from the subject matching engine...
    auto subscriptionCount = m_subjectMatchingEngine.removeSubscription(
        header.getSubject(),
        pSocket->getName());

    // If we have no subscriptions remaining for this subject AND the unsubscribe came
    // from a client (not a mesh peer) then we forward the unsubscribe to the mesh...
    if (subscriptionCount == 0 && pSocket->getIsMeshPeer() == false)
    {
        relayToMesh(pBuffer);
    }
}

// Called when we receive a SEND_MESSAGE message.
void ServiceManager::onMessage(const NetworkMessageHeader& header, Socket* pSocket, BufferPtr pBuffer)
{
    // We find the clients which have subscriptions to the message subject...
    auto subscriptionInfos = m_subjectMatchingEngine.getMatchingSubscriptionInfos(header.getSubject());

    // We send the update to each 'target' matching the subscription.
    // 1. We send to all non-mesh clients.
    // 
    // 2. We forward the message to mesh peers ONLY if it originated from a non-mesh client.
    //    We do not relay messages from one peer gateway to others.
    // 
    // 3. We forward the message only once to each mesh peer, even if the subject matches multiple 
    //    subscriptions (eg, wildcards). It is the peer gateway's job to fan out the update at its end.
    for (const auto& pSubscriptionInfo : subscriptionInfos)
    {
        const auto& pTargetSocket = pSubscriptionInfo->getSocket();

        // 1. We send to non-mesh clients.
        if (pTargetSocket->getIsMeshPeer() == false)
        {
            pTargetSocket->write(pBuffer, pSubscriptionInfo->getSubscriptionID());
            pTargetSocket->setAlreadyUpdated(true);
        }

        // 2 & 3. We send to mesh peers.
        // - If the target is a mesh peer
        // - If the update came from a non-mesh client
        // - If we have not already sent the update to the target
        if (pTargetSocket->getIsMeshPeer() == true
            &&
            pSocket->getIsMeshPeer() == false
            &&
            pTargetSocket->getAlreadyUpdated() == false)
        {
            pTargetSocket->write(pBuffer, pSubscriptionInfo->getSubscriptionID());
            pTargetSocket->setAlreadyUpdated(true);
        }
    }

    // We clear the already-updated flags on the sockets...
    for (const auto& pSubscriptionInfo : subscriptionInfos)
    {
        const auto& pTargetSocket = pSubscriptionInfo->getSocket();
        pTargetSocket->setAlreadyUpdated(false);
    }
}

// Relays the message / update in the buffer to all mesh peers.
void ServiceManager::relayToMesh(BufferPtr pBuffer)
{
    for (const auto& [key, meshGatewayConnection] : m_meshGatewayConnections_WeAreTheClient)
    {
        meshGatewayConnection.relay(pBuffer);
    }
}

