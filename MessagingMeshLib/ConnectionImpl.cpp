#include "ConnectionImpl.h"
#include "UVLoop.h"
#include "Exception.h"
#include "Socket.h"
#include "Logger.h"
#include "MMUtils.h"
#include "Utils.h"
#include "NetworkMessage.h"
#include "Message.h"
#include "Subscription.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Constructor.
ConnectionImpl::ConnectionImpl(const std::string& hostname, int port, const std::string& service) :
    m_hostname(hostname),
    m_port(port),
    m_service(service)
{
    // We create the UV loop for client messaging...
    auto name = Utils::format("MM-%s", service.c_str());
    m_pUVLoop = UVLoop::create(name);

    // We create the socket to connect to the gateway...
    m_pSocket = Socket::create(m_pUVLoop);
    m_pSocket->setCallback(this);

    // We connect to the gateway...
    m_pUVLoop->marshallEvent(
        [this](uv_loop_t* /*pLoop*/)
        {
            m_pSocket->connect(m_hostname, m_port);
        }
    );

    // We send a CONNECT message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::CONNECT);
    header.setSubject(m_service);
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);

    // We wait for the ACK to confirm that we have connected.
    //
    // This is done in the constructor as we do not want client code to send messages
    // until the Gateway has fully set up the socket at its end and assigned it to
    // the correct thread for the requested service. When the ACK signal has been sent
    // we know that this has been completed.
    auto waitResult = m_ackSignal.waitOne(30.0);
    if (!waitResult)
    {
        throw Exception("Timed out without receive ACK from the Messaging Mesh Gateway");
    }
}

// Destructor.
ConnectionImpl::~ConnectionImpl()
{
    // We unsubscribe from all active subscriptions...
    for (auto& pair : m_subscriptions)
    {
        // We unsubscribe...
        auto subscriptionID = pair.first;
        unsubscribe(subscriptionID, false);

        // We note in the Subscription object that the Connection has closed
        // in case client code is holding these objects after the lifetime of
        // the COnnection.
        auto pSubscription = pair.second;
        pSubscription->resetConnection();
    }
    m_subscriptions.clear();

    // We send a DISCONNECT message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::DISCONNECT);
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);
}

// Sends a message to the specified subject.
void ConnectionImpl::sendMessage(const std::string& subject, const MessagePtr& pMessage) const
{
    // We create a NetworkMessage to send the message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::SEND_MESSAGE);
    header.setSubject(subject);
    networkMessage.setMessage(pMessage);

    // We send the message...
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);
}

// Subscribes to a subject.
// The lifetime of the subscription is the lifetime of the object returned.
SubscriptionPtr ConnectionImpl::subscribe(const std::string& subject, SubscriptionCallback callback)
{
    // We find the next subscription ID...
    auto subscriptionID = m_nextSubscriptionID++;

    // We create an object to manage the subscription. 
    // The subscription will be removed when this object is destructed.
    auto pSubscription = Subscription::create(this, subscriptionID, callback);
    m_subscriptions.insert({ subscriptionID, pSubscription.get()});

    // We send a SUBSCRIBE message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::SUBSCRIBE);
    header.setSubscriptionID(subscriptionID);
    header.setSubject(subject);
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);

    return pSubscription;
}

// Unsubscribes from a subscription.
void ConnectionImpl::unsubscribe(uint32_t subscriptionID, bool removeFromCollection)
{
    // We send an UNSUBSCRIBE message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::UNSUBSCRIBE);
    header.setSubscriptionID(subscriptionID);
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);

    // We remove the subscription from the collection...
    if (removeFromCollection)
    {
        m_subscriptions.erase(subscriptionID);
    }
}

// Called when data has been received on the socket.
// Called on the UV loop thread.
void ConnectionImpl::onDataReceived(Socket* /*pSocket*/, BufferPtr pBuffer)
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
        case NetworkMessageHeader::Action::ACK:
            onAck();
            break;

        case NetworkMessageHeader::Action::SEND_MESSAGE:
            onSendMessage(header, pBuffer);
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when a socket has been disconnected.
void ConnectionImpl::onDisconnected(Socket* pSocket)
{
    try
    {
        // RSSTODO: REMOVE THIS!!!
        Logger::info(Utils::format("ConnectionImpl::onDisconnected, socket-name=%s", pSocket->getName().c_str()));
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when we see the ACK message from the Gateway.
void ConnectionImpl::onAck()
{
    try
    {
        // We signal that the ACK has been received...
        m_ackSignal.set();
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when we see the SEND_MESSAGE message from the Gateway.
void ConnectionImpl::onSendMessage(const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We check if we have a subscription for this update...
    auto it = m_subscriptions.find(header.getSubscriptionID());
    if (it == m_subscriptions.end())
    {
        Logger::warn("NO SUBSCRIPTION FOR UPDATE! RSSTODO: REMOVE THIS");
        // We do not have a subscription for this update.
        return;
    }
    auto pSubscription = it->second;

    // We deserialize the message. The buffer should already have its position
    // at the right point for this as the header was deserialized previously...
    auto pMessage = Message::create();
    pMessage->deserialize(*pBuffer);
    pSubscription->callback(header.getSubject(), header.getReplySubject(), pMessage);
}
