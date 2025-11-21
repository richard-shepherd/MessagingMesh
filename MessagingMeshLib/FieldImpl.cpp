#include "FieldImpl.h"
#include "Buffer.h"
#include "Exception.h"
using namespace MessagingMesh;

// Checks that the data-type we hold is an the type specified.
// (This is a macro so that we can stringify the enum name in exception text.)
#define CHECK_DATA_TYPE(x) if(m_dataType != x) throw Exception("Field '" + m_name + "' is not a " + #x)

// Constructor.
FieldImpl::FieldImpl() :
    m_dataType(Field::NOT_SET),
    m_dataNumeric({ 0 })
{
}

// Destructor.
FieldImpl::~FieldImpl()
{
}

// Gets the field's name.
const std::string& FieldImpl::getName() const
{
    return m_name;
}

// Sets the field's name.
void FieldImpl::setName(const std::string& name)
{
    m_name = name;
}

// Gets the string held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
const std::string& FieldImpl::getString() const
{
    CHECK_DATA_TYPE(Field::STRING);
    return m_dataString;
}

// Sets the field to hold a string.
void FieldImpl::setString(const std::string& value)
{
    m_dataType = Field::STRING;
    m_dataString = value;
}

// Gets the signed int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int32_t FieldImpl::getSignedInt32() const
{
    CHECK_DATA_TYPE(Field::SIGNED_INT32);
    return m_dataNumeric.Int32;
}

// Sets the field to hold a signed int32.
void FieldImpl::setSignedInt32(int32_t value)
{
    m_dataType = Field::SIGNED_INT32;
    m_dataNumeric.Int32 = value;
}

// Gets the unsigned int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint32_t FieldImpl::getUnsignedInt32() const
{
    CHECK_DATA_TYPE(Field::UNSIGNED_INT32);
    return m_dataNumeric.UInt32;
}

// Sets the field to hold an unsigned int32.
void FieldImpl::setUnsignedInt32(uint32_t value)
{
    m_dataType = Field::UNSIGNED_INT32;
    m_dataNumeric.UInt32 = value;
}

// Gets the signed int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int64_t FieldImpl::getSignedInt64() const
{
    CHECK_DATA_TYPE(Field::SIGNED_INT64);
    return m_dataNumeric.Int64;
}

// Sets the field to hold a signed int64.
void FieldImpl::setSignedInt64(int64_t value)
{
    m_dataType = Field::SIGNED_INT64;
    m_dataNumeric.Int64 = value;
}

// Gets the unsigned int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint64_t FieldImpl::getUnsignedInt64() const
{
    CHECK_DATA_TYPE(Field::UNSIGNED_INT64);
    return m_dataNumeric.UInt64;
}

// Sets the field to hold an unsigned int64.
void FieldImpl::setUnsignedInt64(uint64_t value)
{
    m_dataType = Field::UNSIGNED_INT64;
    m_dataNumeric.UInt64 = value;
}

// Gets the double held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
double FieldImpl::getDouble() const
{
    CHECK_DATA_TYPE(Field::DOUBLE);
    return m_dataNumeric.Double;
}

// Sets the field to hold a double.
void FieldImpl::setDouble(double value)
{
    m_dataType = Field::DOUBLE;
    m_dataNumeric.Double = value;
}

// Gets the message held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
const ConstMessagePtr& FieldImpl::getMessage() const
{
    CHECK_DATA_TYPE(Field::MESSAGE);
    return m_dataMessage;
}

// Sets the field to hold a message.
void FieldImpl::setMessage(const ConstMessagePtr& value)
{
    m_dataType = Field::MESSAGE;
    m_dataMessage = value;
}

// Serializes the field to the current position of the buffer.
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

    case Field::UNSIGNED_INT32:
        buffer.write_uint32(m_dataNumeric.UInt32);
        break;

    case Field::SIGNED_INT64:
        buffer.write_int64(m_dataNumeric.Int64);
        break;

    case Field::UNSIGNED_INT64:
        buffer.write_uint64(m_dataNumeric.UInt64);
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

// Deserializes the field from the current position in the buffer.
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

    case Field::UNSIGNED_INT32:
        m_dataNumeric.UInt32 = buffer.read_uint32();
        break;

    case Field::SIGNED_INT64:
        m_dataNumeric.Int64 = buffer.read_int64();
        break;

    case Field::UNSIGNED_INT64:
        m_dataNumeric.UInt64 = buffer.read_uint64();
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
