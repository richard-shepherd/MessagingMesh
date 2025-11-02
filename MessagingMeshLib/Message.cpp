#include "Message.h"
#include "MessageImpl.h"
using namespace MessagingMesh;

Message::Message() :
    m_pImpl(std::make_unique<MessageImpl>())
{
}

// Destructor.
Message::~Message() = default;

const ConstFieldPtr& Message::getField(const std::string& name) const
{
    return m_pImpl->getField(name);
}

void Message::addField(const std::string& name, const std::string& value)
{
    m_pImpl->addField(name, value);
}

void Message::addField(const std::string& name, int32_t value)
{
    m_pImpl->addField(name, value);
}

void Message::addField(const std::string& name, double value)
{
    m_pImpl->addField(name, value);
}

void Message::addField(const std::string& name, const ConstMessagePtr& value)
{
    m_pImpl->addField(name, value);
}

void Message::serialize(Buffer& buffer) const
{
    m_pImpl->serialize(buffer);
}

void Message::deserialize(Buffer& buffer)
{
    m_pImpl->deserialize(buffer);
}
