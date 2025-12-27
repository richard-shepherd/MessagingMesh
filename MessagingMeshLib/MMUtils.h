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
    /// Static utility functions for internal use by messaging mesh.
    /// NOTE: Utilities to be shared with clients go into the Utils class.
    /// </summary>
    class MMUtils
    {
    // Public functions...
    public:
        // Sends a network-message to the socket. 
        // Returns the number of bytes sent on the network.
        static int32_t sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket);

        // Sends a network-message to the socket.
        // Returns the number of bytes sent on the network.
        static int32_t sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket);

        // Splits the input string on the delimiter and returns a vector of tokens.
        static VecToken tokenize(std::string_view, char delimiter);

        // Creates a GUID and returns it in its base64 string format.
        static std::string createGUID();

        // Gets the hostname of the current process.
        // NOTE: Replaces any '.' in the hostname with '-' by default.
        static std::string getHostname(bool replaceDotWithDash = true);

        // Gets the IPV4 address for the hostname specified.
        // If the hostname is empty we return the IP address of the running process.
        static std::string getIPAddress(const std::string& hostname = "");

        // Gets a default client ID of "[executable name] [command-line params]".
        static std::string getDefaultClientID();

        // Gets the executable name.
        static std::string getExecutableName();

        // Gets the command-line parameters.
        static std::string getCommandLineParameters();
    };
} // namespace





