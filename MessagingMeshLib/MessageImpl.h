#pragma once
#include <optional>
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
        void deserialize(const Buffer& buffer);

    // Field add and get methods...
    public:
        // Tries to get a field by name,
        std::optional<FieldPtr> tryGetField(const std::string& name) const;

        // Gets a field by name.
        // Throws a MessagingMesh::Exception if the field is not in the message.
        FieldPtr getField(const std::string& name);

        // Returns a list of all fields in the message.
        // NOTE: This is a copy of the list held by the message, so changes to the returned list
        //       will not change the fields held by the message.
        std::vector<FieldPtr> getAllFields() const;

        // Adds a string field to the message. 
        void addString(const std::string& name, const std::string& value);

        // Adds a signed int32 field to the message. 
        void addSignedInt32(const std::string& name, int32_t value);

        // Adds an unsigned int32 field to the message. 
        void addUnsignedInt32(const std::string& name, uint32_t value);

        // Adds a signed int64 field to the message. 
        void addSignedInt64(const std::string& name, int64_t value);

        // Adds an unsigned int64 field to the message. 
        void addUnsignedInt64(const std::string& name, uint64_t value);

        // Adds a double field to the message. 
        void addDouble(const std::string& name, double value);

        // Adds a message field to the message. 
        void addMessage(const std::string& name, const MessagePtr& value);

        // Adds a bool field to the message. 
        void addBool(const std::string& name, bool value);

        // Adds a BLOB field to the message. 
        void addBLOB(const std::string& name, const BLOBPtr& value);

    // Private functions...
    private:
        // Adds a field to the message, setting its value using the valueSetter function (lambda).
        void addField(const std::string& name, std::function<void(const FieldPtr&)> valueSetter);

    // Private data...
    private:
        // Vector of fields in the message, in the order they were added...
        std::vector<FieldPtr> m_fields;

        // Map of field name to the first field with that name...
        std::unordered_map<std::string, FieldPtr> m_mapNameToField;
    };
} // namespace

