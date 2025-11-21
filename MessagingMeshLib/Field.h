#pragma once
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class FieldImpl;

    // Holds one field in a message.
    class Field
    {
    // Public types...
    public:
        // Types of data which can be held in a field.
        enum DataType
        {
            NOT_SET,
            STRING,
            SIGNED_INT32,
            UNSIGNED_INT32,
            SIGNED_INT64,
            DOUBLE,
            MESSAGE
        };
        
    // Public methods...
    public:
        // Creates a Field instance.
        static FieldPtr create() { return FieldPtr(new Field()); }

        // Destructor.
        ~Field();

        // Gets the field's name.
        const std::string& getName() const;

        // Sets the field's name.
        void setName(const std::string& name);

        // Serializes the field to the current position of the buffer.
        void serialize(Buffer& buffer) const;

        // Deserializes the field from the current position in the buffer.
        void deserialize(Buffer& buffer);

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

        // Gets the double held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        double getDouble() const;

        // Sets the field to hold a double.
        void setDouble(double value);

        // Gets the message held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        const ConstMessagePtr& getMessage() const;

        // Sets the field to hold a message.
        void setMessage(const ConstMessagePtr& value);
        
    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Field::create() to create an instance.
        Field();

    // Implementation...
    private:
        FieldImpl* m_pImpl;
    };
} // namespace

