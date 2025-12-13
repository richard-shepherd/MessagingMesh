#include "Connection.h"
#include "ConnectionImpl.h"
using namespace MessagingMesh;

// Constructor.
Connection::Connection(const ConnectionParams& connectionParams) :
    m_pImpl(std::make_unique<ConnectionImpl>(connectionParams, *this))
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
// Returns the number of bytes sent on the network.
int32_t Connection::sendMessage(const MessagePtr& pMessage, const std::string& subject, const std::string& replySubject)
{
    return m_pImpl->sendMessage(pMessage, subject, replySubject);
}

// Sends a blocking request to the subject specified. Returns the reply or 
// nullptr if the request times out.
MessagePtr Connection::sendRequest(const std::string& subject, const MessagePtr& pMessage, double timeoutSeconds)
{
    return m_pImpl->sendRequest(subject, pMessage, timeoutSeconds);
}

// Subscribes to a subject.
// The lifetime of the subscription is the lifetime of the object returned.
SubscriptionPtr Connection::subscribe(const std::string& subject, SubscriptionCallback callback, void* tag)
{
    return m_pImpl->subscribe(subject, callback, tag);
}

// Processes messages in the queue. Waits for the specified time for messages to be available.
void Connection::processMessageQueue(int millisecondsTimeout)
{
    m_pImpl->processMessageQueue(millisecondsTimeout);
}

// Unblocks the current processMessageQueue() call without waiting for its timeout to elapse.
void Connection::wakeUp()
{
    m_pImpl->wakeUp();
}
