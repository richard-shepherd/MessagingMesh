#pragma once
#include <vector>
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessage;
    class Socket;

    /// <summary>
    /// Static utility functions for messaging mesh.
    /// </summary>
    class MMUtils
    {
    public:
        // Vector of tokens.
        using VecToken = std::vector<std::string>;

    // Public functions...
    public:
        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket);

        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket);

        // Splits the input string on the delimiter and returns a vector of tokens.
        static VecToken tokenize(const std::string& s, char delimiter);
    };
} // namespace


