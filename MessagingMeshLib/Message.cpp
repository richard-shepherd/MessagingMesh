#include "Message.h"
#include "Field.h"
#include "MessageImpl.h"
using namespace MessagingMesh;

// Constructor.
// NOTE: The constructor is private. Use Message::create() to create an instance.
Message::Message() :
    m_pImpl(std::make_unique<MessageImpl>())
{
}

// Destructor.
Message::~Message() = default;

// Gets a field by name.
// Throws a MessagingMesh::Exception if the field is not in the message.
const ConstFieldPtr& Message::getField(const std::string& name) const
{
    return m_pImpl->getField(name);
}

// Adds a string field to the message. 
void Message::addString(const std::string& name, const std::string& value)
{
    m_pImpl->addString(name, value);
}

// Gets the string value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
const std::string& Message::getString(const std::string& name) const
{
    return getField(name)->getString();
}

// Adds a signed int32 field to the message. 
void Message::addSignedInt32(const std::string& name, int32_t value)
{
    m_pImpl->addSignedInt32(name, value);
}

// Gets the signed int32 value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
int32_t Message::getSignedInt32(const std::string& name) const
{
    return getField(name)->getSignedInt32();
}

// Adds an unsigned int32 field to the message. 
void Message::addUnsignedInt32(const std::string& name, uint32_t value)
{
    m_pImpl->addUnsignedInt32(name, value);
}

// Gets the unsigned int32 value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
uint32_t Message::getUnsignedInt32(const std::string& name) const
{
    return getField(name)->getUnsignedInt32();
}

// Adds a signed int64 field to the message. 
void Message::addSignedInt64(const std::string& name, int64_t value)
{
    m_pImpl->addSignedInt64(name, value);
}

// Gets the signed int64 value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
int64_t Message::getSignedInt64(const std::string& name) const
{
    return getField(name)->getSignedInt64();
}

// Adds a double field to the message. 
void Message::addDouble(const std::string& name, double value)
{
    m_pImpl->addDouble(name, value);
}

// Gets the double value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
double Message::getDouble(const std::string& name) const
{
    return getField(name)->getDouble();
}

// Adds a message field to the message. 
void Message::addMessage(const std::string& name, const ConstMessagePtr& value)
{
    m_pImpl->addMessage(name, value);
}

// Gets the sub-message for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
const ConstMessagePtr& Message::getMessage(const std::string& name) const
{
    return getField(name)->getMessage();
}

// Serializes the message to the current position in the buffer.
void Message::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

// Deserializes the message from the current position in the buffer.
void Message::deserialize(Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
