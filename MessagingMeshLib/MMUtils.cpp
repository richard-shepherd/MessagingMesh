#include <string>
#include <filesystem>
#include <sstream>

// For default Client ID (made from executable name and command-line)...
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN    
    #include <Windows.h>
    #include <shellapi.h>
    #include <combaseapi.h>
    #include <wincrypt.h>
    #pragma comment(lib, "Shell32.lib")
    #pragma comment(lib, "Ole32.lib")
    #pragma comment(lib, "Crypt32.lib")
#else
    #include <unistd.h>
#endif

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
    CryptBinaryToStringA(
        guidBytes, 
        sizeof(GUID),
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

// Gets a default client ID of "[executable name] [command-line params]".
std::string MMUtils::getDefaultClientID()
{
    try
    {
        auto executableName = getExecutableName();
        auto commandLineParams = getCommandLineParameters();

        if (commandLineParams.empty())
        {
            return executableName;
        }
        return executableName + " " + commandLineParams;
    }
    catch (...)
    {
        return "[unknown-client-ID]";
    }
}

// Gets the executable name.
std::string MMUtils::getExecutableName()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD size = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (size == 0 || size == MAX_PATH)
    {
        return "unknown";
    }

    std::filesystem::path exePath(buffer);
    return exePath.stem().string(); // Filename without extension
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1)
    {
        return "unknown";
    }
    buffer[len] = '\0';

    std::filesystem::path exePath(buffer);
    return exePath.stem().string();
#endif
}

// Gets the command-line parameters.
std::string MMUtils::getCommandLineParameters()
{
#ifdef _WIN32
    // Get the full command line
    LPWSTR cmdLine = GetCommandLineW();

    // Parse into arguments
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
    if (argv == nullptr)
    {
        return "";
    }

    // Skip first argument (executable path) and join the rest
    std::ostringstream oss;
    for (int i = 1; i < argc; ++i)
    {
        if (i > 1) oss << " ";

        // Convert wide string to narrow string
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string arg(size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, arg.data(), size, nullptr, nullptr);

        oss << arg;
    }

    LocalFree(argv);
    return oss.str();
#else
    // On Linux, read from /proc/self/cmdline
    std::ifstream cmdlineFile("/proc/self/cmdline");
    if (!cmdlineFile)
    {
        return "";
    }

    std::string cmdline;
    std::getline(cmdlineFile, cmdline, '\0'); // Skip executable (first null-terminated string)

    std::ostringstream oss;
    bool first = true;
    std::string arg;
    while (std::getline(cmdlineFile, arg, '\0'))
    {
        if (!first) oss << " ";
        oss << arg;
        first = false;
    }

    return oss.str();
#endif
}
