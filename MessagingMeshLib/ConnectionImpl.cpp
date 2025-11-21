#include "ConnectionImpl.h"
#include <format>
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
ConnectionImpl::ConnectionImpl(const ConnectionParams& connectionParams, Connection& connection) :
    m_connection(connection)
{
    // We create the UV loop for client messaging...
    auto name = std::format("MM-{}", connectionParams.Service);
    m_pUVLoop = UVLoop::create(name);

    // We create the socket to connect to the gateway...
    m_pSocket = Socket::create(m_pUVLoop);
    m_pSocket->setCallback(this);

    // We connect to the gateway...
    m_pUVLoop->marshallEvent(
        [&connectionParams, this](uv_loop_t* /*pLoop*/)
        {
            m_pSocket->connect(connectionParams.GatewayHost, connectionParams.GatewayPort);
        }
    );

    // We send a CONNECT message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::CONNECT);
    header.setSubject(connectionParams.Service);
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
    for (const auto& [subscriptionID, subscriptionInfo] : m_subscriptionsByID)
    {
        unsubscribe(subscriptionID);
    }
    m_subscriptionsByID.clear();
    m_subscriptionsBySubject.clear();

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

// Sends a blocking request to the subject specified. 
// Returns the reply or nullptr if the request times out.
MessagePtr ConnectionImpl::sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds)
{
    MessagePtr pResultMessage = nullptr;

    // We create a unique inbox for the reply...
    auto inbox = createInbox();

    // We create an auto reset event to block while waiting for the reply...
    AutoResetEvent autoResetEvent;

    // We subscribe to the inbox...
    auto pSubscription = subscribe(
        inbox,
        [&pResultMessage, &autoResetEvent](Connection& /*connection*/, const std::string& /*subject*/, const std::string& /*replySubject*/, MessagePtr pMessage, void* /*tag*/)
        {
            // Called on the UV thread when we receive a reply.
            // We note the result and signal that we have received it.
            pResultMessage = pMessage;
            autoResetEvent.set();
        }
    );

    // We create a NetworkMessage to send the request...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::SEND_MESSAGE);
    header.setSubject(subject);
    header.setReplySubject(inbox);
    networkMessage.setMessage(pMessage);

    // We send the message...
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);

    // We block on the auto reset event, waiting for the result...
    autoResetEvent.waitOne(timeoutSeconds);

    // We return the result message. This will be nullptr if the request timed out
    // or the request's result if it did not...
    return pResultMessage;
}

// Subscribes to a subject.
// The lifetime of the subscription is the lifetime of the object returned.
SubscriptionPtr ConnectionImpl::subscribe(const std::string& subject, SubscriptionCallback callback, void* tag)
{
    // We create the subscription-callback-info for this subscription...
    auto pCallbackInfo = new Subscription::CallbackInfo();
    pCallbackInfo->Callback = callback;
    pCallbackInfo->Tag = tag;

    uint32_t subscriptionID;
    bool makeGatewaySubscription = false;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionsMutex);

        // We check if we are already subscribed to this subject...
        auto it = m_subscriptionsBySubject.find(subject);
        if (it == m_subscriptionsBySubject.end())
        {
            // We are not subscribed to this subject, so we create a subscription ID for it...
            subscriptionID = m_nextSubscriptionID++;
            m_subscriptionsBySubject.insert({ subject, subscriptionID });

            // We create the subscription-info and associate it with the subscription-ID...
            SubscriptionInfo subscriptionInfo;
            subscriptionInfo.Subject = subject;
            subscriptionInfo.SubscriptionID = subscriptionID;
            m_subscriptionsByID.insert({ subscriptionID, subscriptionInfo });

            // We invalidate the cache used when processing messages and calling back to clients...
            m_onSendMessageCacheInvalidated = true;

            // We note that we need to subscribe via the gateway...
            makeGatewaySubscription = true;
        }

        // We add the callback-info to the subscription...
        auto& subscriptionInfo = m_subscriptionsByID[subscriptionID];
        subscriptionInfo.CallbackInfos.push_back(pCallbackInfo);
    }

    // We subscribe to the gateway if needed... 
    if (makeGatewaySubscription)
    {
        // We send a SUBSCRIBE message...
        NetworkMessage networkMessage;
        auto& header = networkMessage.getHeader();
        header.setAction(NetworkMessageHeader::Action::SUBSCRIBE);
        header.setSubscriptionID(subscriptionID);
        header.setSubject(subject);
        MMUtils::sendNetworkMessage(networkMessage, m_pSocket);
    }

    // We return a subscription object to manage the lifetime of the subscription...
    return Subscription::create(this, subject, pCallbackInfo);
}

// Unsubscribes from the subscription ID specified.
void ConnectionImpl::unsubscribe(uint32_t subscriptionID)
{
    // We send an UNSUBSCRIBE message...
    NetworkMessage networkMessage;
    auto& header = networkMessage.getHeader();
    header.setAction(NetworkMessageHeader::Action::UNSUBSCRIBE);
    header.setSubscriptionID(subscriptionID);
    MMUtils::sendNetworkMessage(networkMessage, m_pSocket);
}

// Releases a subscription.
void ConnectionImpl::releaseSubscription(const std::string& subject, const Subscription::CallbackInfo* pCallbackInfo)
{
    std::lock_guard<std::mutex> lock(m_subscriptionsMutex);

    // We find the subscription ID...
    auto it_subject = m_subscriptionsBySubject.find(subject);
    if (it_subject == m_subscriptionsBySubject.end())
    {
        // We do not have a subscription for this subject...
        return;
    }
    auto& subscriptionID = it_subject->second;

    // We find the subscription-info...
    auto it_subscriptionID = m_subscriptionsByID.find(subscriptionID);
    if (it_subscriptionID == m_subscriptionsByID.end())
    {
        // We do not have a subscription for the subscription ID...
        return;
    }
    auto& callbackInfos = it_subscriptionID->second.CallbackInfos;

    // We remove the callback...
    auto it_callbackInfo = std::find(callbackInfos.begin(), callbackInfos.end(), pCallbackInfo);
    if (it_callbackInfo != callbackInfos.end())
    {
        callbackInfos.erase(it_callbackInfo);
    }

    // If there are no callbacks left, we clean up the maps...
    if (callbackInfos.empty())
    {
        m_subscriptionsByID.erase(it_subscriptionID);
        m_subscriptionsBySubject.erase(it_subject);
    }

    // We invalidate the cache used when processing messages and calling back to clients...
    m_onSendMessageCacheInvalidated = true;
}


// Called when data has been received on the socket.
// Called on the UV loop thread.
void ConnectionImpl::onDataReceived(Socket* /*pSocket*/, BufferPtr pBuffer)
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

        case NetworkMessageHeader::Action::SEND_MESSAGE:
            onSendMessage(header, pBuffer);
            break;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when a socket has been disconnected.
void ConnectionImpl::onDisconnected(Socket* pSocket)
{
    try
    {
        // RSSTODO: REMOVE THIS!!!
        Logger::info(std::format("ConnectionImpl::onDisconnected, socket-name={}", pSocket->getName()));
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
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
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when we see the SEND_MESSAGE message from the Gateway.
void ConnectionImpl::onSendMessage(const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We check if subscriptions have changed, which will invalidate our cache of subscription-infos...
    auto cacheInvalidated = m_onSendMessageCacheInvalidated.exchange(false);
    if (cacheInvalidated)
    {
        m_onSendMessageCache.clear();
    }

    // We check if we have the subscription in the cache...
    auto it_cache = m_onSendMessageCache.find(header.getSubscriptionID());
    if (it_cache != m_onSendMessageCache.end())
    {
        // We have the subscription-info in the cache...
        const auto& subsciptionInfo = it_cache->second;
        const auto& callbackInfos = subsciptionInfo.CallbackInfos;
        performSubscriptionCallbacks(callbackInfos, header, pBuffer);
    }
    else
    {
        // We do not have the subsciption-info in the cache, so we look it up from the main collection...
        SubscriptionInfo subscriptionInfo;
        {
            std::lock_guard<std::mutex> lock(m_subscriptionsMutex);

            // We check if we have a subscription for this update...
            auto it = m_subscriptionsByID.find(header.getSubscriptionID());
            if (it == m_subscriptionsByID.end())
            {
                // We do not have a subscription for this update.
                // This may be because the client has removed the subscription while messages from the gateway
                // are still in flight to it.
                return;
            }
            subscriptionInfo = it->second;  // RSSTODO: This is copying the data which we would like not to do.
        }

        // We add it to the cache and perform the callbacks...
        m_onSendMessageCache.insert({ header.getSubscriptionID(), subscriptionInfo });
        performSubscriptionCallbacks(subscriptionInfo.CallbackInfos, header, pBuffer);
    }
}

// Calls back for subscriptions to the message described by the header and buffer.
void ConnectionImpl::performSubscriptionCallbacks(const VecCallbackInfo& callbackInfos, const NetworkMessageHeader& header, BufferPtr pBuffer)
{
    // We deserialize the message. The buffer should already have its position
    // at the right point for this as the header was deserialized previously...
    auto pMessage = Message::create();
    pMessage->deserialize(*pBuffer);

    // We call the registered callbacks...
    for (const auto& pCallbackInfo : callbackInfos)
    {
        pCallbackInfo->Callback(m_connection, header.getSubject(), header.getReplySubject(), pMessage, pCallbackInfo->Tag);
    }
}


// Returns a unique inbox name.
std::string ConnectionImpl::createInbox()
{
    auto guid = MMUtils::createGUID();
    return std::format("_INBOX.{}", guid);
}
