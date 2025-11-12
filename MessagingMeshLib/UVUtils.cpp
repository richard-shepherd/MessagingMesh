#include "UVUtils.h"
#include <format>
#include "Utils.h"
#include "Logger.h"
#include "Exception.h"
#include "OSSocketHolder.h"
using namespace MessagingMesh;

// Gets peer IP info for a tcp handle.
// (Peer info is the address and port of the remote end of the socket connection.)
UVUtils::IPInfo UVUtils::getPeerIPInfo(uv_tcp_t* pTCPHandle)
{
    // We get the socket info...
    sockaddr_storage peerInfo;
    int peerInfoSize = sizeof(sockaddr_storage);
    uv_tcp_getpeername(pTCPHandle, (sockaddr*)&peerInfo, &peerInfoSize);

    // We find the hostname/ip-address and service/port...
    char hostname[NI_MAXHOST];
    char service[NI_MAXSERV];
    auto status = getnameinfo(
        (struct sockaddr*)&peerInfo,
        sizeof(struct sockaddr_storage),
        hostname,
        NI_MAXHOST,
        service,
        NI_MAXSERV,
        NI_NUMERICSERV);

    // We return the data in an IPInfo structure...
    IPInfo ipInfo;
    if (status == 0)
    {
        ipInfo.Hostname = hostname;
        ipInfo.Service = service;
    }
    else
    {
        Logger::error(std::format("getPeerInfo: getnameinfo error: {}", status));
    }
    return ipInfo;
}

// Allocates buffer memory for a UV read from a socket.
void UVUtils::allocateBufferMemory(uv_handle_t* /*handle*/, size_t suggested_size, uv_buf_t* pBuffer)
{
    pBuffer->base = new char[suggested_size];
    pBuffer->len = (ULONG)suggested_size;
}

// Releases memory for a buffer.
void UVUtils::releaseBufferMemory(const uv_buf_t* pBuffer)
{
    delete[] pBuffer->base;
}

// Allocates a write request.
UVUtils::WriteRequest* UVUtils::allocateWriteRequest(BufferPtr pBuffer)
{
    return new WriteRequest(pBuffer);
}

// Allocates a write request.
UVUtils::WriteRequest* UVUtils::allocateWriteRequest(size_t bufferSize)
{
    return new WriteRequest(bufferSize);
}

// Releases a write request.
void UVUtils::releaseWriteRequest(WriteRequest* pWriteRequest)
{
    delete pWriteRequest;
}

// Duplicates the socket.
// Note: This has different implementations depending on the OS.
OSSocketHolderPtr UVUtils::duplicateSocket(const uv_os_sock_t& socket)
{
#ifdef WIN32
    auto pSocketHolder = OSSocketHolder::create();
    pSocketHolder->setSocket(duplicateSocket_Windows(socket));
    return pSocketHolder;
#endif
}

// Duplicates the socket when compiling for Windows.
uv_os_sock_t UVUtils::duplicateSocket_Windows(const uv_os_sock_t& socket)
{
    // We get the socket info...
    WSAPROTOCOL_INFO info;
    auto duplicateSocketStatus = WSADuplicateSocket(socket, GetCurrentProcessId(), &info);
    if (duplicateSocketStatus != 0)
    {
        auto error = WSAGetLastError();
        throw Exception(std::format("WSADuplicateSocket failed: {}", error));
    }

    // We duplicate the socket...
    auto newSocket = WSASocket(
        FROM_PROTOCOL_INFO,
        FROM_PROTOCOL_INFO,
        FROM_PROTOCOL_INFO,
        &info, 
        0, 
        WSA_FLAG_OVERLAPPED);
    if (newSocket == INVALID_SOCKET)
    {
        auto error = WSAGetLastError();
        throw Exception(std::format("WSASocket (dup) failed: {}", error));
    }

    // We return the duplicated socket...
    return (uv_os_sock_t)newSocket;
}

// Sets the thread name.
void UVUtils::setThreadName(const std::string& threadName)
{
    uv_thread_setname(threadName.c_str());
}

// Gets the name of the current thread.
std::string UVUtils::getThreadName()
{
    char threadName[128];
    auto threadID = uv_thread_self();
    uv_thread_getname(&threadID, &threadName[0], sizeof(threadName));
    return std::string(threadName);
}
