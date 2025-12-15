#pragma once
#include <optional>
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
            UNSIGNED_INT64,
            DOUBLE,
            MESSAGE,
            BOOL,
            BLOB
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
        void deserialize(const Buffer& buffer);

    // Getters and setters for field types...
    public:
        // Gets the string held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        const std::string& getString() const;

        // Sets the field to hold a string.
        void setString(const std::string& value);

        // Tries to get the string held by the field.
        std::optional<std::reference_wrapper<const std::string>> tryGetString() const;

        // Gets the signed int32 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        int32_t getSignedInt32() const;

        // Sets the field to hold a signed int32.
        void setSignedInt32(int32_t value);

        // Tries to get the signed int32 held by the field.
        std::optional<int32_t> tryGetSignedInt32() const;

        // Gets the unsigned int32 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        uint32_t getUnsignedInt32() const;

        // Sets the field to hold an unsigned int32.
        void setUnsignedInt32(uint32_t value);

        // Tries to get the unsigned int32 held by the field.
        std::optional<uint32_t> tryGetUnsignedInt32() const;

        // Gets the signed int64 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        int64_t getSignedInt64() const;

        // Sets the field to hold a signed int64.
        void setSignedInt64(int64_t value);

        // Tries to get the signed int64 held by the field.
        std::optional<int64_t> tryGetSignedInt64() const;

        // Gets the unsigned int64 held by the field.
        // Throws a MessagingMesh::Exception if the field does not hold this type.
        uint64_t getUnsignedInt64() const;

        // Sets the field to hold an unsigned int64.
        void setUnsignedInt64(uint64_t value);

        // Tries to get the unsigned int64 held by the field.
        std::optional<uint64_t> tryGetUnsignedInt64() const;

        // Gets the double held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        double getDouble() const;

        // Sets the field to hold a double.
        void setDouble(double value);

        // Tries to get the double held by the field.
        std::optional<double> tryGetDouble() const;

        // Gets the message held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        const ConstMessagePtr& getMessage() const;

        // Sets the field to hold a message.
        void setMessage(const ConstMessagePtr& value);

        // Tries to get the Message held by the field.
        std::optional<ConstMessagePtr> tryGetMessage() const;

        // Gets the bool held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        bool getBool() const;

        // Sets the field to hold a bool.
        void setBool(bool value);

        // Tries to get the bool held by the field.
        std::optional<bool> tryGetBool() const;

        // Gets the BLOB held by the field.
        // Throws a MessagingMesh::Exception  if the field does not hold this type.
        const ConstBLOBPtr& getBLOB() const;

        // Sets the field to hold a BLOB.
        void setBLOB(const ConstBLOBPtr& value);

        // Tries to get the BLOB held by the field.
        std::optional<ConstBLOBPtr> tryGetBLOB() const;

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

