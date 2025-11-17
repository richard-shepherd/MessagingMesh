#pragma once
#include <string>
#include "uv.h"
#include "SharedAliases.h"
#include "ThreadsafeConsumableVector.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkData;
    class UVLoop;

    /// <summary>
    /// Manages a socket.
    /// 
    /// Can either be a client socket making a connection to a server
    /// or a server socket listening for client connections.
    /// </summary>
    class Socket
    {
    public:
        // Interface for socket callbacks.
        class ICallback
        {
        public:
            // Called when a new client connection has been made to a listening socket.
            // Called on the UV loop thread.
            virtual void onNewConnection(SocketPtr pClientSocket) = 0;

            // Called when data has been received on the socket.
            // Called on the UV loop thread.
            virtual void onDataReceived(Socket* pSocket, BufferPtr pBuffer) = 0;

            // Called when a socket has been disconnected.
            virtual void onDisconnected(Socket* pSocket) = 0;

            // Called when the movement of the socket to a new UV loop has been completed.
            virtual void onMoveToLoopComplete(Socket* pSocket) = 0;
        };

    // Public methods...
    public:
        // Creates a Socket instance to be managed by the uv loop specified.
        static SocketPtr create(UVLoopPtr pUVLoop) { return SocketPtr(new Socket(pUVLoop)); }

        // Destructor.
        ~Socket();

        // Gets the socket's name.
        const std::string& getName() const { return m_name; }

        // Returns true if the UV loop passed in is the same one managing this socket.
        bool isSameUVLoop(const UVLoopPtr& pUVLoop) const { return pUVLoop == m_pUVLoop; }

        // Sets the callback.
        void setCallback(ICallback* pCallback);

        // Connects a server socket to listen on the specified port.
        void listen(int port);

        // Connects the socket by accepting a listen request received by the server.
        void accept(uv_stream_t* server);

        // Connects a client socket to the hostname and port specified.
        void connect(const std::string& hostname, int port);

        // Queues data to be written to the socket.
        // Can be called from any thread, not just from the uv loop thread.
        // Queued writes will be coalesced into one network update.
        void write(BufferPtr pBuffer, uint32_t subscriptionIDOverride = 0);

        // Moves the socket to be managed by the UV loop specified.
        void moveToLoop(UVLoopPtr pLoop);

    // Private types...
    private:

        // Context information used when moving a socket from one
        // UVLoop to another.
        struct move_socket_t
        {
            Socket* self = nullptr;
            OSSocketHolderPtr pNewOSSocket;
            UVLoopPtr pNewUVLoop;
        };

        // Context information used when connecting to a socket using (hostname, port).
        struct connect_hostname_t
        {
            Socket* self = nullptr;
            int port = 0;
        };

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Socket::create() to create an instance.
        Socket(UVLoopPtr pUVLoop);

        // Creates the UV socket.
        // Note: This is not done in the constructor, as that can be called outside
        //       the UV loop. It is only called from functions inside the loop.
        void createSocket();

        // Connects a client socket to the IP address and port specified.
        void connectIP(const std::string& ipAddress, int port);

        // Called when DNS resolution has completed for a hostname.
        void onDNSResolution(uv_getaddrinfo_t* pRequest, int status, struct addrinfo* pAddressInfo);

        // Called when a socket is connected to set up reading and writing.
        void onSocketConnected();

        // Called at the client side when a client connect request has completed.
        void onConnectCompleted(uv_connect_t* pRequest, int status);

        // Called at the server side when a new client connection has been received
        // on a listening socket.
        void onNewConnection(uv_stream_t* server, int status);

        // Called when data has been received on a socket.
        void onDataReceived(uv_stream_t* pClientStream, ssize_t bufferSize, const uv_buf_t* pBuffer);

        // Called when a write request has completed.
        void onWriteCompleted(uv_write_t* pRequest, int status);

        // Sends network messages for all queued writes.
        void processQueuedWrites();

        // Called after the original socket is closed as part of moving the socket to another UV loop.
        void moveToLoop_onSocketClosed(move_socket_t* pMoveInfo);

        // Connects to the (duplicated) socket specified.
        // Note: This is called on the thread for the UVLoop to which we are moving the socket.
        void moveToLoop_registerDuplicatedSocket(UVLoopPtr pUVLoop, OSSocketHolderPtr pOSSocketHolder);

    // Private data...
    private:
        // The socket's name (made from its connection info).
        std::string m_name;

        // The uv loop managing the socket.
        UVLoopPtr m_pUVLoop;

        // True when the socket is connected.
        bool m_connected;

        // The object on which we call callbacks.
        ICallback* m_pCallback;

        // UV socket handle.
        // Note: This is not a unique_ptr as we need to delete it asynchronously from the Socket destructor.
        uv_tcp_t* m_pSocket;

        // The message being currently read (possibly across multiple onDataReceived callbacks).
        BufferPtr m_pCurrentMessage;

        // Data queued for writing.
        struct BufferInfo
        {
            BufferInfo(BufferPtr b, uint32_t s) : pBuffer(b), subscriptionIDOverride(s) {}
            BufferPtr pBuffer = nullptr;
            uint32_t subscriptionIDOverride = 0;
        };
        ThreadsafeConsumableVector<BufferInfo> m_queuedWrites;

    // Constants...
    private:
        // The maximum backlog of unprocessed incoming connections.
        const int MAX_INCOMING_CONNECTION_BACKLOG = 128;
    };
} // namespace
