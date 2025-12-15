#include "Field.h"
#include "FieldImpl.h"
#include "SharedAliases.h"
using namespace MessagingMesh;

// Constructor.
// NOTE: The constructor is private. Use Field::create() to create an instance.
Field::Field()
: m_pImpl(new FieldImpl())
{
}

// Destructor.
Field::~Field()
{
    delete m_pImpl;
}

// Gets the field's name.
const std::string& Field::getName() const
{
    return m_pImpl->getName();
}

// Sets the field's name.
void Field::setName(const std::string& name)
{
    m_pImpl->setName(name);
}

// Gets the string held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
const std::string& Field::getString() const
{
    return m_pImpl->getString();
}

// Sets the field to hold a string.
void Field::setString(const std::string& value)
{
    m_pImpl->setString(value);
}

// Tries to get the string held by the field.
std::optional<std::reference_wrapper<const std::string>> Field::tryGetString() const
{
    return m_pImpl->tryGetString();
}

// Gets the signed int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int32_t Field::getSignedInt32() const
{
    return m_pImpl->getSignedInt32();
}

// Sets the field to hold a signed int32.
void Field::setSignedInt32(int32_t value)
{
    m_pImpl->setSignedInt32(value);
}

// Tries to get the signed int32 held by the field.
std::optional<int32_t> Field::tryGetSignedInt32() const
{
    return m_pImpl->tryGetSignedInt32();
}

// Gets the unsigned int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint32_t Field::getUnsignedInt32() const
{
    return m_pImpl->getUnsignedInt32();
}

// Sets the field to hold a unsigned int32.
void Field::setUnsignedInt32(uint32_t value)
{
    m_pImpl->setUnsignedInt32(value);
}

// Tries to get the unsigned int32 held by the field.
std::optional<uint32_t> Field::tryGetUnsignedInt32() const
{
    return m_pImpl->tryGetUnsignedInt32();
}

// Gets the signed int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int64_t Field::getSignedInt64() const
{
    return m_pImpl->getSignedInt64();
}

// Sets the field to hold a signed int64.
void Field::setSignedInt64(int64_t value)
{
    m_pImpl->setSignedInt64(value);
}

// Tries to get the signed int64 held by the field.
std::optional<int64_t> Field::tryGetSignedInt64() const
{
    return m_pImpl->tryGetSignedInt64();
}

// Gets the unsigned int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint64_t Field::getUnsignedInt64() const
{
    return m_pImpl->getUnsignedInt64();
}

// Sets the field to hold an unsigned int64.
void Field::setUnsignedInt64(uint64_t value)
{
    m_pImpl->setUnsignedInt64(value);
}

// Tries to get the unsigned int64 held by the field.
std::optional<uint64_t> Field::tryGetUnsignedInt64() const
{
    return m_pImpl->tryGetUnsignedInt64();
}

// Gets the double held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
double Field::getDouble() const
{
    return m_pImpl->getDouble();
}

// Sets the field to hold a double.
void Field::setDouble(double value)
{
    m_pImpl->setDouble(value);
}

// Tries to get the double held by the field.
std::optional<double> Field::tryGetDouble() const
{
    return m_pImpl->tryGetDouble();
}

// Gets the message held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
const ConstMessagePtr& Field::getMessage() const
{
    return m_pImpl->getMessage();
}

// Sets the field to hold a message.
void Field::setMessage(const ConstMessagePtr& value)
{
    m_pImpl->setMessage(value);
}

// Tries to get the Message held by the field.
std::optional<ConstMessagePtr> Field::tryGetMessage() const
{
    return m_pImpl->tryGetMessage();
}

// Gets the bool held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
bool Field::getBool() const
{
    return m_pImpl->getBool();
}

// Sets the field to hold a bool.
void Field::setBool(bool value)
{
    m_pImpl->setBool(value);
}

// Tries to get the bool held by the field.
std::optional<bool> Field::tryGetBool() const
{
    return m_pImpl->tryGetBool();
}

// Gets the BLOB held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
const ConstBLOBPtr& Field::getBLOB() const
{
    return m_pImpl->getBLOB();
}

// Sets the field to hold a BLOB.
void Field::setBLOB(const ConstBLOBPtr& value)
{
    m_pImpl->setBLOB(value);
}

// Tries to get the BLOB held by the field.
std::optional<ConstBLOBPtr> Field::tryGetBLOB() const
{
    return m_pImpl->tryGetBLOB();
}

// Serializes the field to the current position of the buffer.
void Field::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

// Deserializes the field from the current position in the buffer.
void Field::deserialize(const Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
