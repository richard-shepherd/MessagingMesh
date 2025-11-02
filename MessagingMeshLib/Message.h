#pragma once
#include <string>
#include <memory>
#include "SharedPointers.h"

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
        // Constructor.
        // NOTE: The constructor is private. Use Message::create() to create an instance.
        Message();

    // Implementation...
    private:
        std::unique_ptr<MessageImpl> m_pImpl;
    };

} // namespace

