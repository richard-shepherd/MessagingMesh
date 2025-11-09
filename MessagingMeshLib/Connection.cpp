#include "Connection.h"
#include "ConnectionImpl.h"
using namespace MessagingMesh;

// Constructor.
Connection::Connection(const std::string& hostname, int port, const std::string& service) :
    m_pImpl(std::make_unique<ConnectionImpl>(hostname, port, service))
{
}

// Destructor.
Connection::~Connection() = default;

// Returns a unique inbox name.
std::string Connection::createInbox()
{
    return m_pImpl->createInbox();
}

// Sends a message to the specified subject.
void Connection::sendMessage(const std::string& subject, const MessagePtr& pMessage)
{
    m_pImpl->sendMessage(subject, pMessage);
}

// Sends a blocking request to the subject specified. Returns the reply or 
// nullptr if the request times out.
MessagePtr Connection::sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds)
{
    return m_pImpl->sendRequest(subject, pMessage, timeoutSeconds);
}

// Subscribes to a subject.
// The lifetime of the subscription is the lifetime of the object returned.
SubscriptionPtr Connection::subscribe(const std::string& subject, SubscriptionCallback callback)
{
    return m_pImpl->subscribe(subject, callback);
}
