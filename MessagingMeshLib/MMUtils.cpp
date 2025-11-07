#include "MMUtils.h"
#include "NetworkMessage.h"
#include "Socket.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Sends a network-message to the socket.
void MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket)
{
    sendNetworkMessage(networkMessage, pSocket.get());
}

// Sends a network-message to the socket.
void MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket)
{
    // We serialize the message and send it...
    auto pBuffer = Buffer::create();
    networkMessage.serialize(*pBuffer);
    pSocket->write(pBuffer);
}

// Splits the input string on the delimiter and returns a vector of tokens.
MMUtils::VecToken MMUtils::tokenize(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    if (s.length() == 0)
    {
        return tokens;
    }

    auto lastIndex = s.length() - 1;
    auto start = 0;
    for (auto i = 0; i <= lastIndex; ++i)
    {
        if (s[i] == delimiter)
        {
            tokens.push_back(s.substr(start, i - start));
            start = i + 1;
        }
        else if (i == lastIndex)
        {
            tokens.push_back(s.substr(start, i - start + 1));
        }
    }
    return tokens;
}
