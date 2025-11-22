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

// Serializes the field to the current position of the buffer.
void Field::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

// Deserializes the field from the current position in the buffer.
void Field::deserialize(Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
