#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessage;
    class Socket;

    // Token...
    using Token = std::string_view;

    // Vector of tokens.
    using VecToken = std::vector<Token>;

    /// <summary>
    /// Static utility functions for messaging mesh.
    /// </summary>
    class MMUtils
    {
    // Public functions...
    public:
        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket);

        // Sends a network-message to the socket.
        static void sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket);

        // Splits the input string on the delimiter and returns a vector of tokens.
        static VecToken tokenize(std::string_view, char delimiter);
    };
} // namespace


