#pragma once
#include <string>
#include <memory>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class MessageImpl;

    // A message which can be sent via the Messaging Mesh.
    // Holds a vector of named Fields which can be accessed by
    // name or by index. You can add multiple fields with the
    // same name.
    class Message
    {
    // Public methods...
    public:
        // Creates a Message instance.
        static MessagePtr create() { return MessagePtr(new Message()); }

        // Destructor.
        ~Message();

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

        // Gets the string value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        const std::string& getString(const std::string& name) const;

        // Adds a signed int32 field to the message. 
        void addSignedInt32(const std::string& name, int32_t value);

        // Gets the signed int32 value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        int32_t getSignedInt32(const std::string& name) const;

        // Adds an unsigned int32 field to the message. 
        void addUnsignedInt32(const std::string& name, uint32_t value);

        // Gets the unsigned int32 value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        uint32_t getUnsignedInt32(const std::string& name) const;

        // Adds a signed int64 field to the message. 
        void addSignedInt64(const std::string& name, int64_t value);

        // Gets the signed int64 value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        int64_t getSignedInt64(const std::string& name) const;

        // Adds an unsigned int64 field to the message. 
        void addUnsignedInt64(const std::string& name, uint64_t value);

        // Gets the unsigned int64 value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        uint64_t getUnsignedInt64(const std::string& name) const;

        // Adds a double field to the message. 
        void addDouble(const std::string& name, double value);

        // Gets the double value for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        double getDouble(const std::string& name) const;

        // Adds a message field to the message. 
        void addMessage(const std::string& name, const ConstMessagePtr& value);

        // Gets the sub-message for the field specified.
        // Throws a MessagingMesh::Exception if the field is not in the message or is not of the requested type.
        const ConstMessagePtr& getMessage(const std::string& name) const;

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Message::create() to create an instance.
        Message();

    // Implementation...
    private:
        std::unique_ptr<MessageImpl> m_pImpl;
    };

} // namespace

