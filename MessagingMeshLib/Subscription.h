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
    /// Subscription objects are returned by Connection.subscribe(). The subscription
    /// will be removed when the object is destructed.
    /// </summary>
    class Subscription
    {
    // Public types...
    public:
        // Callback info.
        struct CallbackInfo
        {
            // The callback.
            SubscriptionCallback Callback = nullptr;

            // A tag / closure passed to the callback.
            void* Tag = nullptr;
        };

    // Public methods:
    public:
        // Creates a Subscription instance.
        static SubscriptionPtr create(ConnectionImpl* pConnection, const std::string& subject, const CallbackInfo* pCallbackInfo, uint32_t subscriptionID) 
        { 
            return SubscriptionPtr(new Subscription(pConnection, subject, pCallbackInfo, subscriptionID));
        }

        // Destructor.
        ~Subscription();

        // Gets the subcription ID.
        uint32_t getSubscriptionID() const { return m_subscriptionID; }

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Subscription::create() to create an instance.
        Subscription(ConnectionImpl* pConnection, const std::string& subject, const CallbackInfo* callbackInfo, uint32_t subscriptionID);

    // Private data...
    private:
        // Construction params...
        ConnectionImpl* m_pConnection;
        std::string m_subject;
        const CallbackInfo* m_pCallbackInfo;
        uint32_t m_subscriptionID;
    };
} // namespace


