#pragma once
#include <cstdint>
#include <string>

namespace MessagingMesh
{
    // Forward declarations...
    class Buffer;

    /// <summary>
    /// Header sent with NetworkMessages.
    /// </summary>
    class NetworkMessageHeader
    {
    // Public types...
    public:
        // Enum for network-message actions.
        enum class Action
        {
            NONE,
            CONNECT,
            DISCONNECT,
            ACK,
            SUBSCRIBE,
            UNSUBSCRIBE,
            SEND_MESSAGE,
            CONNECT_MESH_PEER
        };

    // Public methods...
    public:
        // Constructor.
        NetworkMessageHeader();

        // Serializes the network message header to the current position of the buffer.
        void serialize(Buffer& buffer) const;

        // Deserializes the network message header from the current position in the buffer.
        void deserialize(Buffer& buffer);

        // Sets the subscription ID.
        void setSubscriptionID(uint32_t subscriptionID) { m_subscriptionID = subscriptionID; }

        // Gets the subscription ID.
        uint32_t getSubscriptionID() const { return m_subscriptionID; }

        // Sets the subject.
        void setSubject(const std::string& subject) { m_subject = subject; }

        // Gets the subject.
        const std::string& getSubject() const { return m_subject; }

        // Sets the reply subject.
        void setReplySubject(const std::string& replySubject) { m_replySubject = replySubject; }

        // Gets the reply subject.
        const std::string& getReplySubject() const { return m_replySubject; }

        // Sets the action.
        void setAction(Action action) { m_action = action; }

        // Gets the action.
        Action getAction() const { return m_action; }


    // Private data...
    private:
        // Client ID for a subscription. Helps match subscriptions to callbacks.
        uint32_t m_subscriptionID = 0;

        // Subject for subscriptions and message-sending.
        // Also holds the service-name for CONNECT and DISCONNECT messages.
        std::string m_subject;

        // Reply subject for synchronous requests...
        std::string m_replySubject;
        
        // Action...
        Action m_action = Action::NONE;
    };
} // namespace

