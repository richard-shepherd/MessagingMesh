#include "NetworkMessage.h"
#include "Message.h"
#include "Logger.h"
using namespace MessagingMesh;

// Constructor.
NetworkMessage::NetworkMessage()
{
}

// Destructor.
NetworkMessage::~NetworkMessage()
{
}

// Gets the message.
MessagePtr NetworkMessage::getMessage() const
{ 
    createMessageIfItDoesNotExist();
    return m_pMessage; 
}

// Sets the message.
void NetworkMessage::setMessage(const MessagePtr& pMessage) 
{
    m_pMessage = pMessage;
}

// Serializes the network message to the current position of the buffer.
void NetworkMessage::serialize(Buffer& buffer) const
{
    // Header...
    m_header.serialize(buffer);

    // Message.
    createMessageIfItDoesNotExist();
    m_pMessage->serialize(buffer);
}

// Deserializes the network message from the current position in the buffer.
void NetworkMessage::deserialize(Buffer& buffer)
{
    deserializeHeader(buffer);
    deserializeMessage(buffer);
}

// Deserializes the header from the current position in the buffer.
void NetworkMessage::deserializeHeader(Buffer& buffer)
{
    m_header.deserialize(buffer);
}

// Deserializes the message from the current position in the buffer.
void NetworkMessage::deserializeMessage(Buffer& buffer)
{
    createMessageIfItDoesNotExist();
    m_pMessage->deserialize(buffer);
}

// Creates the message we hold if it does not already exist.
void NetworkMessage::createMessageIfItDoesNotExist() const
{
    if (!m_pMessage)
    {
        m_pMessage = Message::create();
    }
}
