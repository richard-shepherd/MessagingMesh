#include "Subscription.h"
#include "ConnectionImpl.h"
using namespace MessagingMesh;

// Constructor.
Subscription::Subscription(ConnectionImpl* pConnection, const std::string& subject, const CallbackInfo* pCallbackInfo, uint32_t subscriptionID) :
    m_pConnection(pConnection),
    m_subject(subject),
    m_pCallbackInfo(pCallbackInfo),
    m_subscriptionID(subscriptionID)
{
}

// Destructor.
Subscription::~Subscription()
{
    // We release the subscription...
    m_pConnection->releaseSubscription(m_subject, m_pCallbackInfo);
    delete m_pCallbackInfo;
}

