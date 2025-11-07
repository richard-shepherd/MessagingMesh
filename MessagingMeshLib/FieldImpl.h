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

        // Deserialized the field from the current position in the buffer.
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

    // Private data...
    private:
        std::string m_name;
        Field::DataType m_dataType;

        // Data for the various supported types...
        union NumericDataUnion
        {
            int32_t Int32;
            double Double;
        };
        NumericDataUnion m_dataNumeric;
        std::string m_dataString;
        ConstMessagePtr m_dataMessage = nullptr;
    };
} // namespace

