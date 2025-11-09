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



    //// We convert it to a string...
    //wchar_t guidStr[39]; // GUID string is 38 chars + null terminator
    //StringFromGUID2(guid, guidStr, 39);

    //// Convert wide string to std::string
    //int size = WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, nullptr, 0, nullptr, nullptr);
    //std::string result(size - 1, 0);
    //WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, &result[0], size, nullptr, nullptr);

    //return result;
#endif
}
