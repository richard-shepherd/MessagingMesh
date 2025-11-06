#include "Subscription.h"
#include "ConnectionImpl.h"
using namespace MessagingMesh;

// Constructor.
Subscription::Subscription(ConnectionImpl* pConnection, uint32_t subscriptionID, SubscriptionCallback callback) :
    m_pConnection(pConnection),
    m_subscriptionID(subscriptionID),
    m_callback(callback)
{
}

// Destructor.
Subscription::~Subscription()
{
    // We check if the connection has already been destructed.
    // If not, we call it to unsubscribe.
    ConnectionImpl* pConnection;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pConnection = m_pConnection;
    }
    if (pConnection)
    {
        pConnection->unsubscribe(m_subscriptionID, true);
    }
}

// Calls the callback with the data provided.
void Subscription::callback(const std::string& subject, const std::string& replySubject, MessagePtr pMessage)
{
    m_callback(subject, replySubject, pMessage);
}

// Sets m_pConnection to nullptr when the Connection is closed, to avoid calling
// into it if the lifetime of this object is longer than that of the Connection.
void Subscription::resetConnection()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pConnection = nullptr;
}

