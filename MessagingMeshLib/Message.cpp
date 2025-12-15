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
ConstFieldPtr Message::getConstField(const std::string& name) const
{
    return m_pImpl->getConstField(name);
}

// Tries to get a field by name,
std::optional<ConstFieldPtr> Message::tryGetConstField(const std::string& name) const
{
    return m_pImpl->tryGetConstField(name);
}

// Gets a field by name.
// Throws a MessagingMesh::Exception if the field is not in the message.
FieldPtr Message::getField(const std::string& name)
{
    return m_pImpl->getField(name);
}

// Returns a list of all fields in the message.
// NOTE: This is a copy of the list held by the message, so changes to the returned list
//       will not change the fields held by the message.
std::vector<ConstFieldPtr> Message::getAllFields() const
{
    return m_pImpl->getAllFields();
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
    return getConstField(name)->getString();
}

// Tries to get the string value for the field specified.
std::optional<std::reference_wrapper<const std::string>> Message::tryGetString(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetString();
    }
    return std::nullopt;
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
    return getConstField(name)->getSignedInt32();
}

// Tries to get the signed int32 value for the field specified.
std::optional<int32_t> Message::tryGetSignedInt32(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetSignedInt32();
    }
    return std::nullopt;
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
    return getConstField(name)->getUnsignedInt32();
}

// Tries to get the unsigned int32 value for the field specified.
std::optional<uint32_t> Message::tryGetUnsignedInt32(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetUnsignedInt32();
    }
    return std::nullopt;
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
    return getConstField(name)->getSignedInt64();
}

// Tries to get the signed int64 value for the field specified.
std::optional<int64_t> Message::tryGetSignedInt64(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetSignedInt64();
    }
    return std::nullopt;
}

// Adds an unsigned int64 field to the message. 
void Message::addUnsignedInt64(const std::string& name, uint64_t value)
{
    m_pImpl->addUnsignedInt64(name, value);
}

// Gets the unsigned int64 value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
uint64_t Message::getUnsignedInt64(const std::string& name) const
{
    return getConstField(name)->getUnsignedInt64();
}

// Tries to get the unsigned int64 value for the field specified.
std::optional<uint64_t> Message::tryGetUnsignedInt64(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetUnsignedInt64();
    }
    return std::nullopt;
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
    return getConstField(name)->getDouble();
}

// Tries to get the double value for the field specified.
std::optional<double> Message::tryGetDouble(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetDouble();
    }
    return std::nullopt;
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
    return getConstField(name)->getMessage();
}

// Tries to get the Message value for the field specified.
std::optional<ConstMessagePtr> Message::tryGetMessage(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetMessage();
    }
    return std::nullopt;
}

// Adds a bool field to the message. 
void Message::addBool(const std::string& name, bool value)
{
    m_pImpl->addBool(name, value);
}

// Gets the bool value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
bool Message::getBool(const std::string& name) const
{
    return getConstField(name)->getBool();
}

// Tries to get the bool value for the field specified.
std::optional<bool> Message::tryGetBool(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetBool();
    }
    return std::nullopt;
}

// Adds a BLOB field to the message. 
void Message::addBLOB(const std::string& name, const ConstBLOBPtr& value)
{
    m_pImpl->addBLOB(name, value);
}

// Gets the BLOB value for the field specified.
// Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
const ConstBLOBPtr& Message::getBLOB(const std::string& name) const
{
    return getConstField(name)->getBLOB();
}

// Tries to get the BLOB value for the field specified.
std::optional<ConstBLOBPtr> Message::tryGetBLOB(const std::string& name) const
{
    if (auto field = tryGetConstField(name))
    {
        return (*field)->tryGetBLOB();
    }
    return std::nullopt;
}

// Serializes the message to the current position in the buffer.
void Message::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

// Deserializes the message from the current position in the buffer.
void Message::deserialize(const Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
