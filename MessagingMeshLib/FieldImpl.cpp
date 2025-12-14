#include "FieldImpl.h"
#include "Buffer.h"
#include "Exception.h"
using namespace MessagingMesh;

// Checks that the data-type we hold is an the type specified.
// (This is a macro so that we can stringify the enum name in exception text.)
#define CHECK_DATA_TYPE(x) if(m_dataType != x) [[unlikely]] throw Exception("Field '" + m_name + "' is not a " + #x)

// Constructor.
FieldImpl::FieldImpl() :
    m_dataType(Field::NOT_SET)
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
    return std::get<std::string>(m_data);
}

// Sets the field to hold a string.
void FieldImpl::setString(const std::string& value)
{
    m_dataType = Field::STRING;
    m_data = value;
}

// Gets the signed int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int32_t FieldImpl::getSignedInt32() const
{
    CHECK_DATA_TYPE(Field::SIGNED_INT32);
    return std::get<int32_t>(m_data);
}

// Sets the field to hold a signed int32.
void FieldImpl::setSignedInt32(int32_t value)
{
    m_dataType = Field::SIGNED_INT32;
    m_data = value;
}

// Gets the unsigned int32 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint32_t FieldImpl::getUnsignedInt32() const
{
    CHECK_DATA_TYPE(Field::UNSIGNED_INT32);
    return std::get<uint32_t>(m_data);
}

// Sets the field to hold an unsigned int32.
void FieldImpl::setUnsignedInt32(uint32_t value)
{
    m_dataType = Field::UNSIGNED_INT32;
    m_data = value;
}

// Gets the signed int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
int64_t FieldImpl::getSignedInt64() const
{
    CHECK_DATA_TYPE(Field::SIGNED_INT64);
    return std::get<int64_t>(m_data);
}

// Sets the field to hold a signed int64.
void FieldImpl::setSignedInt64(int64_t value)
{
    m_dataType = Field::SIGNED_INT64;
    m_data = value;
}

// Gets the unsigned int64 held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
uint64_t FieldImpl::getUnsignedInt64() const
{
    CHECK_DATA_TYPE(Field::UNSIGNED_INT64);
    return std::get<uint64_t>(m_data);
}

// Sets the field to hold an unsigned int64.
void FieldImpl::setUnsignedInt64(uint64_t value)
{
    m_dataType = Field::UNSIGNED_INT64;
    m_data = value;
}

// Gets the double held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
double FieldImpl::getDouble() const
{
    CHECK_DATA_TYPE(Field::DOUBLE);
    return std::get<double>(m_data);
}

// Sets the field to hold a double.
void FieldImpl::setDouble(double value)
{
    m_dataType = Field::DOUBLE;
    m_data = value;
}

// Gets the message held by the field.
// Throws a MessagingMesh::Exception if the field does not hold this type.
const ConstMessagePtr& FieldImpl::getMessage() const
{
    CHECK_DATA_TYPE(Field::MESSAGE);
    return std::get<ConstMessagePtr>(m_data);
}

// Sets the field to hold a message.
void FieldImpl::setMessage(const ConstMessagePtr& value)
{
    m_dataType = Field::MESSAGE;
    m_data = value;
}

// Gets the bool held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
bool FieldImpl::getBool() const
{
    CHECK_DATA_TYPE(Field::BOOL);
    return std::get<bool>(m_data);
}

// Sets the field to hold a bool.
void FieldImpl::setBool(bool value)
{
    m_dataType = Field::BOOL;
    m_data = value;
}

// Gets the BLOB held by the field.
// Throws a MessagingMesh::Exception  if the field does not hold this type.
const ConstBLOBPtr& FieldImpl::getBLOB() const
{
    CHECK_DATA_TYPE(Field::BLOB);
    return std::get<ConstBLOBPtr>(m_data);
}

// Sets the field to hold a BLOB.
void FieldImpl::setBLOB(const ConstBLOBPtr& value)
{
    m_dataType = Field::BLOB;
    m_data = value;
}

// Serializes the field to the current position of the buffer.
void FieldImpl::serialize(Buffer& buffer) const
{
    // We serialize the field name...
    buffer.write_string(m_name);

    // We serialize the data type...
    buffer.write_uint8(static_cast<uint8_t>(m_dataType));

    // We serialize the data, depending on the type...
    switch (m_dataType)
    {
    case Field::STRING:
        buffer.write_string(std::get<std::string>(m_data));
        break;

    case Field::SIGNED_INT32:
        buffer.write_int32(std::get<int32_t>(m_data));
        break;

    case Field::UNSIGNED_INT32:
        buffer.write_uint32(std::get<uint32_t>(m_data));
        break;

    case Field::SIGNED_INT64:
        buffer.write_int64(std::get<int64_t>(m_data));
        break;

    case Field::UNSIGNED_INT64:
        buffer.write_uint64(std::get<uint64_t>(m_data));
        break;

    case Field::DOUBLE:
        buffer.write_double(std::get<double>(m_data));
        break;

    case Field::MESSAGE:
        buffer.write_message(std::get<ConstMessagePtr>(m_data));
        break;

    case Field::BOOL:
        buffer.write_bool(std::get<bool>(m_data));
        break;

    case Field::BLOB:
        buffer.write_blob(std::get<ConstBLOBPtr>(m_data));
        break;

    default:
        throw Exception("Field::serialize data-type not handled");
    }
}

// Deserializes the field from the current position in the buffer.
void FieldImpl::deserialize(const Buffer& buffer)
{
    // We deserialize the name...
    m_name = buffer.read_string();

    // We deserialize the data type...
    m_dataType = static_cast<Field::DataType>(buffer.read_uint8());

    // We deserialize the data, depending on the type...
    switch (m_dataType)
    {
    case Field::STRING:
        m_data = buffer.read_string();
        break;

    case Field::SIGNED_INT32:
        m_data = buffer.read_int32();
        break;

    case Field::UNSIGNED_INT32:
        m_data = buffer.read_uint32();
        break;

    case Field::SIGNED_INT64:
        m_data = buffer.read_int64();
        break;

    case Field::UNSIGNED_INT64:
        m_data = buffer.read_uint64();
        break;

    case Field::DOUBLE:
        m_data = buffer.read_double();
        break;

    case Field::MESSAGE:
        m_data = buffer.read_message();
        break;

    case Field::BOOL:
        m_data = buffer.read_bool();
        break;

    case Field::BLOB:
        m_data = buffer.read_blob();
        break;

    default:
        throw Exception("Field::deserialize data-type not handled");
    }
}
