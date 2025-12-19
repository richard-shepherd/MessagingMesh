#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include <queue>
#include <atomic>
#include "SharedAliases.h"
#include "Socket.h"
#include "AutoResetEvent.h"
#include "Callbacks.h"
#include "ConnectionParams.h"
#include "Subscription.h"
#include "ThreadsafeConsumableQueue.h"
#include "NetworkMessageHeader.h"
#include "MessageQueueInfo.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessage;

    /// <summary>
    /// Implementation of the Connection class, ie a client connection
    /// to the messaging-mesh.
    /// </summary>
    class ConnectionImpl : Socket::ICallback
    {
    // Public methods...
    public:
        // Constructor.
        ConnectionImpl(const ConnectionParams& connectionParams, Connection& connection);

        // Destructor.
        ~ConnectionImpl();

        // Gets the library version.
        static const std::string& getVersion();

        // Returns a unique inbox name.
        std::string createInbox();

        // Sends a message to the specified subject.
        // Returns the number of bytes sent on the network.
        int32_t sendMessage(const MessagePtr& pMessage, const std::string& subject, const std::string& replySubject = "") const;

        // Sends a blocking request to the subject specified. 
        // Returns the reply or nullptr if the request times out.
        MessagePtr sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds);

        // Subscribes to a subject.
        // The lifetime of the subscription is the lifetime of the object returned.
        SubscriptionPtr subscribe(const std::string& subject, SubscriptionCallback callback, void* tag = nullptr);

        // Releases a subscription.
        void releaseSubscription(const std::string& subject, const Subscription::CallbackInfo* pCallbackInfo);

        // Processes messages in the queue. Waits for the specified time for messages to be available.
        MessageQueueInfo processMessageQueue(int millisecondsTimeout, int maxMessages = -1);

        // Unblocks the current processMessageQueue() call without waiting for its timeout to elapse.
        void wakeUp();

    // Socket::ICallback implementation
    private:
        // Called when a new client connection has been made to a listening socket.
        // Called on the UV loop thread.
        void onNewConnection(SocketPtr /*pClientSocket*/) {}

        // Called when data has been received on the socket.
        // Called on the UV loop thread.
        void onDataReceived(Socket* pSocket, BufferPtr pBuffer);

        // Called when the connection status has changed.
        virtual void onConnectionStatusChanged(Socket* pSocket, Socket::ConnectionStatus connectionStatus, const std::string& message);

        // Called when the movement of the socket to a new UV loop has been completed.
        void onMoveToLoopComplete(Socket* /*pSocket*/) {}

    // Private types...
    private:
        using VecCallbackInfo = std::vector<const Subscription::CallbackInfo*>;

    // Private functions...
    private:
        // Called when we see the ACK message from the Gateway.
        void onAck();

        // Called when we see a SEND_MESSAGE message from the Gateway.
        void onGatewayMessage(const NetworkMessageHeader& header, BufferPtr pBuffer);

        // Processes a message from the gateway, calling client callbacks if we have subscriptions set up for it.
        void processGatewayMessage(const NetworkMessageHeader& header, BufferPtr pBuffer);

        // Unsubscribes from the subscription ID specified.
        void unsubscribe(uint32_t subscriptionID);

        // Calls back for subscriptions to the message described by the header and buffer.
        void performSubscriptionCallbacks(const VecCallbackInfo& callbackInfos, const NetworkMessageHeader& header, BufferPtr pBuffer);

        // Processes all messages from the backlog and the message queue.
        size_t processMessageQueue_AllMessages(int millisecondsTimeout);

        // Processes queued messages from the backlog and the message queue, up to the maximum
        // number of messages specified.
        // Returns the number of messages processed.
        size_t processMessageQueue_MaxMessages(int millisecondsTimeout, int maxMessages);

    // Private data...
    private:
        // Connection params...
        ConnectionParams m_connectionParams;

        // The (non-impl) Connection. Sent to subscription callbacks.
        Connection& m_connection;

        // UV loop for client messaging...
        UVLoopPtr m_pUVLoop;

        // Socket connection to the gateway...
        SocketPtr m_pSocket;

        // Waits for the ACK signal...
        AutoResetEvent m_ackSignal;

        // Threadsafe subscription ID.
        // Note: This starts at 1, which means we can use 0 to indicate an invalid subscription ID.
        std::atomic<uint32_t> m_nextSubscriptionID = 1;

        // Info for a subscription to a subject.
        struct SubscriptionInfo
        {
            std::string Subject;
            uint32_t SubscriptionID;
            std::vector<const Subscription::CallbackInfo*> CallbackInfos;
        };

        // Subscription-infos keyed by subscription-ID, used when setting up subscriptions...
        std::unordered_map<uint32_t, SubscriptionInfo> m_subscriptionsByID;

        // A cache of the map above used in the onSendMessage function (on its own thread)...
        std::unordered_map<uint32_t, SubscriptionInfo> m_onSendMessageCache;
        std::atomic<bool> m_onSendMessageCacheInvalidated;

        // Map of subject names to subscription IDs...
        std::unordered_map<std::string, uint32_t> m_subscriptionsBySubject;

        // A mutex for subscriptions...
        std::mutex m_subscriptionsMutex;

        // Subscription IDs used by blocking sendRequest() and a mutex for them...
        std::set<uint32_t> m_requestSubscriptionIDs;
        std::mutex m_requestSubscriptionIDsMutex;

        // Messages received from the gateway, queued for processMessageQueue.
        struct QueuedMessage
        {
            QueuedMessage(const NetworkMessageHeader& header, const BufferPtr& buffer) : Header(header), pBuffer(buffer) {}
            NetworkMessageHeader Header;
            BufferPtr pBuffer;
        };
        ThreadsafeConsumableQueue<QueuedMessage> m_queuedMessages;

        // Message backlog if maxMessages is passed to processMessageQueue() and
        // not all messages have been processed.
        std::queue<QueuedMessage> m_messageBacklog;
    };
} // namespace


