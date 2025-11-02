#pragma once
#include <string>
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
        // Returns a std::string created using the string format and variadic arguments.
        static std::string format(const char* format, ...);

        // Returns a time string in the format HH:MM:SS.mmm
        static std::string getTimeString();

        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket);

        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket);
    };
} // namespace


