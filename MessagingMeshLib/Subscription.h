#pragma once
#include <cstdint>
#include <mutex>
#include "Callbacks.h"

namespace MessagingMesh
{
    // Forward declarations...
    class ConnectionImpl;

    /// <summary>
    /// Manages a client subscription.
    /// 
    /// The lifetime of the subscription is the lifetime of this object. When this object
    /// is destructed, the subscription will be removed.
    /// </summary>
    class Subscription
    {
    // Public methods:
    public:
        // Creates a Subscription instance.
        static SubscriptionPtr create(ConnectionImpl* pConnection, uint32_t subscriptionID, SubscriptionCallback callback) 
        { 
            return SubscriptionPtr(new Subscription(pConnection, subscriptionID, callback));
        }

        // Destructor.
        ~Subscription();

        // Calls the callback with the data provided.
        void callback(const std::string& subject, const std::string& replySubject, MessagePtr pMessage);

        // Sets m_pConnection to nullptr when the Connection is closed, to avoid calling
        // into it if the lifetime of this object is longer than that of the Connection.
        void resetConnection();

        // Gets the subscription ID.
        uint32_t getSubscriptionID() const { return m_subscriptionID; }

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Subscription::create() to create an instance.
        Subscription(ConnectionImpl* pConnection, uint32_t subscriptionID, SubscriptionCallback callback);

    // Private data...
    private:
        // Construction params...
        ConnectionImpl* m_pConnection;
        uint32_t m_subscriptionID;
        SubscriptionCallback m_callback;

        // For updates to m_pConnection.
        std::mutex m_mutex;
    };
} // namespace


