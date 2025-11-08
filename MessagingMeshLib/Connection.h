#pragma once
#include <memory>
#include <string>
#include <functional>
#include "SharedAliases.h"
#include "Callbacks.h"

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
        Connection(const std::string& hostname, int port, const std::string& service);

        // Destructor.
        ~Connection();

        // Sends a message to the specified subject.
        void sendMessage(const std::string& subject, const MessagePtr& pMessage);

        // Sends a blocking request to the subject specified. Returns the reply or 
        // nullptr if the request times out.
        MessagePtr sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds);

        // Subscribes to a subject.
        // The lifetime of the subscription is the lifetime of the object returned.
        SubscriptionPtr subscribe(const std::string& subject, SubscriptionCallback callback);
        
    // Private data...
    private:
        // Implementation...
        std::unique_ptr<ConnectionImpl> m_pImpl;
    };
} // namespace


