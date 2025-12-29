#pragma once
#include <string>
#include "Callbacks.h"

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

        // Callback for notifications.
        NotificationCallback NotificationCallback = nullptr;

        // If true constructing a Connection will return before the connection is complete.
        // You can use the notification callback to see when the connection is ready.
        bool ConnectAsynchronously = false;
    };
} // namespace


