#pragma once
#include <memory>
#include <string>
#include "uv.h"
#include "Buffer.h"

namespace MessagingMesh
{
    /// <summary>
    /// Utils for use with libuv.
    /// </summary>
    class UVUtils
    {
    // Public types...
    public:

        // Holds parsed IP info.
        struct IPInfo
        {
            std::string Hostname;  // Hostname or IP address
            std::string Service;   // Service or port
        };

        // A UV write request plus the buffer it is writing.
        struct WriteRequest
        {
            // Constructor from Buffer. 
            // The buffer points to the data in the Buffer, and we hold a reference
            // to the Buffer to ensure that its lifetime matches the write request.
            WriteRequest(BufferPtr pBuffer) :
                write_request{},
                m_pBuffer(pBuffer)
            {
                buffer.base = pBuffer->getBuffer();
                buffer.len = pBuffer->getBufferSize();
            }

            // Constructor specifying a buffer size.
            // We allocate the buffer and release it in the destructor.
            WriteRequest(size_t bufferSize) :
                write_request{},
                m_pBuffer(nullptr)
            {
                buffer.base = new char[bufferSize];
                buffer.len = (ULONG)bufferSize;
            }

            // Destructor.
            ~WriteRequest()
            {
                // If we allocated the buffer (ie, if we are not using the
                // buffer from a Buffer), we release it.
                if (!m_pBuffer)
                {
                    delete[] buffer.base;
                }
            }

            uv_write_t write_request;
            uv_buf_t buffer;
            BufferPtr m_pBuffer;
        };

    // Public functions...
    public:
        // Gets peer IP info for a tcp handle.
        // (Peer info is the address and port of the remote end of the socket connection.)
        static IPInfo getPeerIPInfo(uv_tcp_t* pTCPHandle);

        // Allocates a buffer for a UV read from a socket.
        static void allocateBufferMemory(uv_handle_t* pHandle, size_t suggested_size, uv_buf_t* pBuffer);

        // Releases a buffer.
        static void releaseBufferMemory(const uv_buf_t* pBuffer);

        // Allocates a write request.
        static WriteRequest* allocateWriteRequest(BufferPtr pBuffer);

        // Allocates a write request.
        static WriteRequest* allocateWriteRequest(size_t bufferSize);

        // Releases a write request.
        static void releaseWriteRequest(WriteRequest* pWriteRequest);

        // Duplicates the socket.
        // Note: This has different implementations depending on the OS.
        static OSSocketHolderPtr duplicateSocket(const uv_os_sock_t& socket);

        // Sets the thread name.
        static void setThreadName(const std::string& threadName);

        // Gets the name of the current thread.
        static std::string getThreadName();

    // Private functions...
    private:
        // Duplicates the socket when compiling for Windows.
        static uv_os_sock_t duplicateSocket_Windows(const uv_os_sock_t& socket);
    };
} // namespace

