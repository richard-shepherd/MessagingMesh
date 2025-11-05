#pragma once
#include <cstdint>
#include "GatewaySharedPointers.h"

namespace MessagingMesh
{
    class Socket;

    /// <summary>
    /// Info about a subscription, stored in the subject matching engine's interest graph. 
    /// </summary>
    class SubscriptionInfo
    {
    // Public methods...
    public:
        // Creates a SubscriptionInfo instance.
        static SubscriptionInfoPtr create(Socket* pSocket, uint32_t subscriptionID) 
        { 
            return SubscriptionInfoPtr(new SubscriptionInfo(pSocket, subscriptionID)); 
        }

        // Gets the socket.
        Socket* getSocket() const { return m_pSocket; }

        // Gets the subscription ID.
        uint32_t getSubscriptionID() const { return m_subscriptionID; }

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use SubscriptionInfo::create() to create an instance.
        SubscriptionInfo(Socket* pSocket, uint32_t subscriptionID) :
            m_pSocket(pSocket),
            m_subscriptionID(subscriptionID)
        {
        }

    // Private data...
    private:
        // The socket for the client which made the subscription...
        Socket* m_pSocket;

        // The client's subscription ID...
        uint32_t m_subscriptionID;
    };
} // namespace


