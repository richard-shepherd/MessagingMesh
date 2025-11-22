#pragma once
#include <string>
#include "Field.h"
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Implementation of Field functionality.
    class FieldImpl
    {
    // Public methods...
    public:
        // Constructor.
        FieldImpl();

        // Destructor.
        ~FieldImpl();

        // Gets the field's name.
        const std::string& getName() const;

        // Sets the field's name.
        void setName(const std::string& name);

        // Serializes the field to the current position of the buffer.
        void serialize(Buffer& buffer) const;

        // Deserializes the field from the current position in the buffer.
        void deserialize(const Buffer& buffer);

    // Getters and setters for field types...
    public:
        // Gets the string held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        const std::string& getString() const;

        // Sets the field to hold a string.
        void setString(const std::string& value);

        // Gets the signed int32 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        int32_t getSignedInt32() const;

        // Sets the field to hold a signed int32.
        void setSignedInt32(int32_t value);

        // Gets the unsigned int32 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        uint32_t getUnsignedInt32() const;

        // Sets the field to hold an unsigned int32.
        void setUnsignedInt32(uint32_t value);

        // Gets the signed int64 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        int64_t getSignedInt64() const;

        // Sets the field to hold a signed int64.
        void setSignedInt64(int64_t value);

        // Gets the unsigned int64 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        uint64_t getUnsignedInt64() const;

        // Sets the field to hold an unsigned int64.
        void setUnsignedInt64(uint64_t value);

        // Gets the double held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        double getDouble() const;

        // Sets the field to hold a double.
        void setDouble(double value);

        // Gets the message held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        const ConstMessagePtr& getMessage() const;

        // Sets the field to hold a message.
        void setMessage(const ConstMessagePtr& value);

        // Gets the bool held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        bool getBool() const;

        // Sets the field to hold a bool.
        void setBool(bool value);

    // Private data...
    private:
        std::string m_name;
        Field::DataType m_dataType;

        // Data for the various supported types...
        union NumericDataUnion
        {
            int32_t Int32;
            uint32_t UInt32;
            int64_t Int64;
            uint64_t UInt64;
            double Double;
        };
        NumericDataUnion m_dataNumeric;
        std::string m_dataString;
        ConstMessagePtr m_dataMessage = nullptr;
        bool m_dataBool = false;
    };
} // namespace

