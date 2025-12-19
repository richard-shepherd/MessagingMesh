#pragma once
#include <memory>
#include <string>
#include "SharedAliases.h"
#include "Callbacks.h"
#include "ConnectionParams.h"
#include "MessageQueueInfo.h"

namespace MessagingMesh
{
    // Forward declarations...
    class ConnectionImpl;

    /// <summary>
    /// Client connection to the messaging-mesh.
    /// </summary>
    class Connection
    {
    // Public methods...
    public:
        // Constructor.
        Connection(const ConnectionParams& connectionParams);

        // Destructor.
        ~Connection();

        // Gets the library version.
        static const std::string& getVersion();

        // Returns a unique inbox name.
        std::string createInbox();

        // Sends a message to the specified subject.
        // Returns the number of bytes sent on the network.
        int32_t sendMessage(const MessagePtr& pMessage, const std::string& subject, const std::string& replySubject = "");

        // Sends a blocking request to the subject specified. Returns the reply or 
        // nullptr if the request times out.
        MessagePtr sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds);

        // Subscribes to a subject.
        // The lifetime of the subscription is the lifetime of the object returned.
        [[nodiscard]] SubscriptionPtr subscribe(const std::string& subject, SubscriptionCallback callback, void* tag=nullptr);

        // Processes messages in the queue. Waits for the specified time for messages to be available.
        MessageQueueInfo processMessageQueue(int millisecondsTimeout, int maxMessages = -1);

        // Unblocks the current processMessageQueue() call without waiting for its timeout to elapse.
        void wakeUp();
        
    // Private data...
    private:
        // Implementation...
        std::unique_ptr<ConnectionImpl> m_pImpl;
    };
} // namespace


