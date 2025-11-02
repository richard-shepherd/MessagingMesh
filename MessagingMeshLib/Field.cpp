#include "Field.h"
#include "FieldImpl.h"
#include "SharedPointers.h"
using namespace MessagingMesh;

Field::Field()
: m_pImpl(new FieldImpl())
{
}

Field::~Field()
{
    delete m_pImpl;
}

const std::string& Field::getName() const
{
    return m_pImpl->getName();
}

void Field::setName(const std::string& name)
{
    m_pImpl->setName(name);
}

const std::string& Field::getString() const
{
    return m_pImpl->getString();
}

void Field::setString(const std::string& value)
{
    m_pImpl->setString(value);
}

int32_t Field::getSignedInt32() const
{
    return m_pImpl->getSignedInt32();
}

void Field::setSignedInt32(int32_t value)
{
    m_pImpl->setSignedInt32(value);
}

double Field::getDouble() const
{
    return m_pImpl->getDouble();
}

void Field::setDouble(double value)
{
    m_pImpl->setDouble(value);
}

const ConstMessagePtr& Field::getMessage() const
{
    return m_pImpl->getMessage();
}

void Field::setMessage(const ConstMessagePtr& value)
{
    m_pImpl->setMessage(value);
}

void Field::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

void Field::deserialize(Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
