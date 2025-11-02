#pragma once
#include "SharedPointers.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessage;
    class Socket;

    /// <summary>
    /// Static utility functions for messaging mesh.
    /// </summary>
    class Utils
    {
    // Public functions...
    public:
        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket);

        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket);
    };
} // namespace


