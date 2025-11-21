#include "MessageImpl.h"
#include "Field.h"
#include "Buffer.h"
#include "Exception.h"
using namespace MessagingMesh;

// Constructor.
MessageImpl::MessageImpl()
{
}

// Gets a field by name.
// Throws a MessagingMesh::Exception if the field is not in the message.
const ConstFieldPtr& MessageImpl::getField(const std::string& name) const
{
    auto it = m_mapNameToField.find(name);
    if (it == m_mapNameToField.end())
    {
        throw Exception("Field " + name + " not in message");
    }
    return it->second;
}

// Adds a string field to the message. 
void MessageImpl::addString(const std::string& name, const std::string& value)
{
    addField(name, [&value](const FieldPtr& field) {field->setString(value);});
}

// Adds a signed int32 field to the message. 
void MessageImpl::addSignedInt32(const std::string& name, int32_t value)
{
    addField(name, [&value](const FieldPtr& field) {field->setSignedInt32(value);});
}

// Adds an unsigned int32 field to the message. 
void MessageImpl::addUnsignedInt32(const std::string& name, uint32_t value)
{
    addField(name, [&value](const FieldPtr& field) {field->setUnsignedInt32(value);});
}

// Adds a double field to the message. 
void MessageImpl::addDouble(const std::string& name, double value)
{
    addField(name, [&value](const FieldPtr& field) {field->setDouble(value);});
}

// Adds a message field to the message. 
void MessageImpl::addMessage(const std::string& name, const ConstMessagePtr& value)
{
    addField(name, [&value](const FieldPtr& field) {field->setMessage(value);});
}

/// <summary>
/// Private addField helper used by the public addField methods to create
/// and set up a field and add it to the message.
/// </summary>
void MessageImpl::addField(const std::string& name, std::function<void(const FieldPtr&)> valueSetter)
{
    // We create a field and set its name...
    auto field = Field::create();
    field->setName(name);

    // We call the function (lambda) to set the value...
    valueSetter(field);

    // We add the field to the list of fields...
    m_fields.push_back(field);

    // We add the field to the map of name->first-field-for-name...
    m_mapNameToField.insert({ name, field });
}

// Serializes the message to the current position in the buffer.
void MessageImpl::serialize(Buffer& buffer) const
{
    // We write the number of fields...
    auto fieldCount = static_cast<int32_t>(m_fields.size());
    buffer.write_int32(fieldCount);

    // We write each field...
    for (auto& field : m_fields)
    {
        buffer.write_field(field);
    }
}

// Deserializes the message from the current position in the buffer.
void MessageImpl::deserialize(Buffer& buffer)
{
    // We find the number of fields...
    auto fieldCount = buffer.read_int32();

    // We read each field and add them to the message...
    for (auto i = 0; i < fieldCount; ++i)
    {
        auto field = buffer.read_field();
        m_fields.push_back(field);
        m_mapNameToField.insert({ field->getName(), field });
    }
}
