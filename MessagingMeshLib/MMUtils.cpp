#include "MMUtils.h"
#include "NetworkMessage.h"
#include "Socket.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Sends a network-message to the socket.
// Returns the number of bytes sent on the network.
int32_t MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, SocketPtr pSocket)
{
    return sendNetworkMessage(networkMessage, pSocket.get());
}

// Sends a network-message to the socket.
// Returns the number of bytes sent on the network.
int32_t MMUtils::sendNetworkMessage(const NetworkMessage& networkMessage, Socket* pSocket)
{
    // We serialize the message and send it...
    auto pBuffer = Buffer::create();
    networkMessage.serialize(*pBuffer);
    pSocket->write(pBuffer);
    return pBuffer->getBufferSize();
}

// Splits the input string on the delimiter and returns a vector of tokens.
VecToken MMUtils::tokenize(std::string_view s, char delimiter)
{
    VecToken tokens;
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

// Creates a GUID and returns it in its base64 string format.
std::string MMUtils::createGUID()
{
#ifdef WIN32

    // We create GUID...
    GUID guid;
    CoCreateGuid(&guid);

    const BYTE* guidBytes = reinterpret_cast<const BYTE*>(&guid);

    // Calculate required buffer size
    DWORD base64Length = 0;
    CryptBinaryToStringA(guidBytes, sizeof(GUID),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        nullptr, &base64Length);

    // Convert to base64
    std::string base64(base64Length, '\0');
    CryptBinaryToStringA(guidBytes, sizeof(GUID),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &base64[0], &base64Length);

    // Remove null terminator if present
    if (!base64.empty() && base64.back() == '\0') {
        base64.pop_back();
    }

    return base64;
#endif
}

// Gets the hostname of the current process.
std::string MMUtils::getHostname()
{
    std::string hostname;
    char szHostname[256];
    if (gethostname(szHostname, sizeof(szHostname)) == 0) 
    {
        hostname = szHostname;
    }
    return hostname;
}

// Gets the IPV4 address for the hostname specified.
// If the hostname is empty we return the IP address of the running process.
std::string MMUtils::getIPAddress(const std::string& hostname)
{
    // If the hostname is empty we find the hostname of the running process...
    auto resolvedHostname = hostname;
    if (resolvedHostname.empty())
    {
        resolvedHostname = getHostname();
    }

    // We look up the IP address...
    struct addrinfo *results;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string ipAddress;
    auto status = getaddrinfo(resolvedHostname.c_str(), nullptr, &hints, &results);
    if (status == 0)
    {
        char ip[17];
        uv_ip4_name((struct sockaddr_in*)results->ai_addr, ip, sizeof(ip));
        ipAddress = ip;
        freeaddrinfo(results);
    }
    return ipAddress;
}

