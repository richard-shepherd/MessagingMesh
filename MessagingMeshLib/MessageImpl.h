#pragma once
#include <vector>
#include <map>
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

        // Gets a field by name.
        // Throws a MessagingMesh::Exception if the field is not in the message.
        const ConstFieldPtr& getField(const std::string& name) const;

        // Serializes the message to the current position in the buffer.
        void serialize(Buffer& buffer) const;

        // Deserializes the message from the current position in the buffer.
        void deserialize(Buffer& buffer);

    // Helper methods to add fields of various types...
    public:
        // Adds a string field to the message. 
        void addField(const std::string& name, const std::string& value);

        // Adds a signed int32 field to the message. 
        void addField(const std::string& name, int32_t value);

        // Adds a double field to the message. 
        void addField(const std::string& name, double value);

        // Adds a message field to the message. 
        void addField(const std::string& name, const ConstMessagePtr& value);
    
    // Private functions...
    private:
        // Adds a field to the message, setting its value using the valueSetter function (lambda).
        void addField(const std::string& name, std::function<void(const FieldPtr&)> valueSetter);

    // Private data...
    private:
        // Vector of fields in the message, in the order they were added...
        std::vector<ConstFieldPtr> m_fields;

        // Map of field name to the first field with that name...
        std::map<std::string, ConstFieldPtr> m_mapNameToField;
    };
} // namespace

