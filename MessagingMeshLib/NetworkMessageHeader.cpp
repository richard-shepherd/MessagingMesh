#include "NetworkMessageHeader.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Constructor.
NetworkMessageHeader::NetworkMessageHeader()
{
}

// Serializes the network message header to the current position of the buffer.
void NetworkMessageHeader::serialize(Buffer& buffer) const
{
    // Subscription ID...
    buffer.write_uint32(m_subscriptionID);

    // Subject...
    buffer.write_string(m_subject);

    // Reply subject...
    buffer.write_string(m_replySubject);

    // Action...
    buffer.write_int8(static_cast<int8_t>(m_action));
}

// Deserializes the network message header from the current position in the buffer.
void NetworkMessageHeader::deserialize(Buffer& buffer)
{
    // Subscription ID...
    m_subscriptionID = buffer.read_uint32();

    // Subject...
    m_subject = buffer.read_string();

    // Reply subject...
    m_replySubject = buffer.read_string();

    // Action...
    m_action = static_cast<Action>(buffer.read_int8());
}
