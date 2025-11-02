#include "FieldImpl.h"
#include "Buffer.h"
#include "Exception.h"
using namespace MessagingMesh;

// Checks that the data-type we hold is an the type specified.
// (This is a macro so that we can stringify the enum name in exception text.)
#define CHECK_DATA_TYPE(x) if(m_dataType != x) throw Exception("Field '" + m_name + "' is not a " + #x)

FieldImpl::FieldImpl() :
    m_dataType(Field::NOT_SET),
    m_dataNumeric({ 0 })
{
}

FieldImpl::~FieldImpl()
{
}

void FieldImpl::setName(const std::string& name)
{
    m_name = name;
}

const std::string& FieldImpl::getName() const
{
    return m_name;
}

const std::string& FieldImpl::getString() const 
{
    CHECK_DATA_TYPE(Field::STRING);
    return m_dataString;
}

void FieldImpl::setString(const std::string& value) 
{
    m_dataType = Field::STRING;
    m_dataString = value;
}

int32_t FieldImpl::getSignedInt32() const
{
    CHECK_DATA_TYPE(Field::SIGNED_INT32);
    return m_dataNumeric.Int32;
}

void FieldImpl::setSignedInt32(int32_t value)
{
    m_dataType = Field::SIGNED_INT32;
    m_dataNumeric.Int32 = value;
}

double FieldImpl::getDouble() const
{
    CHECK_DATA_TYPE(Field::DOUBLE);
    return m_dataNumeric.Double;
}

void FieldImpl::setDouble(double value)
{
    m_dataType = Field::DOUBLE;
    m_dataNumeric.Double = value;
}

const ConstMessagePtr& FieldImpl::getMessage() const
{
    CHECK_DATA_TYPE(Field::MESSAGE);
    return m_dataMessage;
}

void FieldImpl::setMessage(const ConstMessagePtr& value)
{
    m_dataType = Field::MESSAGE;
    m_dataMessage = value;
}

void FieldImpl::serialize(Buffer& buffer) const
{
    // We serialize the field name...
    buffer.write_string(m_name);

    // We serialize the data type...
    buffer.write_int8(static_cast<int8_t>(m_dataType));

    // We serialize the data, depending on the type...
    switch (m_dataType)
    {
    case Field::STRING:
        buffer.write_string(m_dataString);
        break;

    case Field::SIGNED_INT32:
        buffer.write_int32(m_dataNumeric.Int32);
        break;

    case Field::DOUBLE:
        buffer.write_double(m_dataNumeric.Double);
        break;

    case Field::MESSAGE:
        buffer.write_message(m_dataMessage);
        break;

    default:
        throw Exception("Field::serialize data-type not handled");
    }
}

void FieldImpl::deserialize(Buffer& buffer)
{
    // We deserialize the name...
    m_name = buffer.read_string();

    // We deserialize the data type...
    m_dataType = static_cast<Field::DataType>(buffer.read_int8());

    // We deserialize the data, depending on the type...
    switch (m_dataType)
    {
    case Field::STRING:
        m_dataString = buffer.read_string();
        break;

    case Field::SIGNED_INT32:
        m_dataNumeric.Int32 = buffer.read_int32();
        break;

    case Field::DOUBLE:
        m_dataNumeric.Double = buffer.read_double();
        break;

    case Field::MESSAGE:
        m_dataMessage = buffer.read_message();
        break;

    default:
        throw Exception("Field::deserialize data-type not handled");
    }
}
