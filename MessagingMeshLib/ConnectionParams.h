#pragma once
#include <string>

namespace MessagingMesh
{
    // Connection parameters.
    struct ConnectionParams
    {
        // Enum for how messages are dispatched to clients.
        enum class MessageDispatch
        {
            // Subscription callbacks are called on the Connection's messaging thread.
            CALLBACK_ON_MESSAGING_MESH_THREAD,

            // Subscription callbacks are manually dispatched on the client thread by calling Connection.processMessageQueue().
            PROCESS_MESSAGE_QUEUE
        };


        // The gateway host or IP address.
        std::string GatewayHost;

        // The gateway port.
        int GatewayPort;

        // The messaging-mesh service to join.
        std::string Service;

        // The client ID.
        std::string ClientID;

        // How subscribed messages are dispatched to client code.
        MessageDispatch MessageDispatch = MessageDispatch::PROCESS_MESSAGE_QUEUE;
    };
} // namespace


