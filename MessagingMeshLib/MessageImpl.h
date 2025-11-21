#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Implementation of Message functionality.
    class MessageImpl
    {
    // Public methods...
    public:
        // Constructor.
        MessageImpl();

        // Destructor.
        ~MessageImpl() = default;

        // Serializes the message to the current position in the buffer.
        void serialize(Buffer& buffer) const;

        // Deserializes the message from the current position in the buffer.
        void deserialize(Buffer& buffer);

    // Field add and get methods...
    public:
        // Gets a field by name.
        // Throws a MessagingMesh::Exception if the field is not in the message.
        const ConstFieldPtr& getField(const std::string& name) const;

        // Adds a string field to the message. 
        void addString(const std::string& name, const std::string& value);

        // Adds a signed int32 field to the message. 
        void addSignedInt32(const std::string& name, int32_t value);

        // Adds a double field to the message. 
        void addDouble(const std::string& name, double value);

        // Adds a message field to the message. 
        void addMessage(const std::string& name, const ConstMessagePtr& value);
    
    // Private functions...
    private:
        // Adds a field to the message, setting its value using the valueSetter function (lambda).
        void addField(const std::string& name, std::function<void(const FieldPtr&)> valueSetter);

    // Private data...
    private:
        // Vector of fields in the message, in the order they were added...
        std::vector<ConstFieldPtr> m_fields;

        // Map of field name to the first field with that name...
        std::unordered_map<std::string, ConstFieldPtr> m_mapNameToField;
    };
} // namespace

