#include "Socket.h"
#include "Logger.h"
#include "Utils.h"
#include "UVUtils.h"
#include "UVLoop.h"
#include "Buffer.h"
#include "OSSocketHolder.h"
#include "Exception.h"
using namespace MessagingMesh;

// Constructor.
// NOTE: The constructor is private. Use Socket::create() to create an instance.
Socket::Socket(UVLoopPtr pUVLoop) :
    m_pUVLoop(pUVLoop),
    m_connected(false),
    m_pCallback(nullptr),
    m_pSocket(nullptr),
    m_pCurrentMessage(nullptr)
{
}

// Destructor.
Socket::~Socket()
{
    Logger::info("Closing socket: " + m_name);

    // The destructor could be called from a different thread than the
    // one running the UV loop, so we marshall the socket close event
    // to the socket's UV loop.
    auto pSocket = m_pSocket;
    m_pUVLoop->marshallEvent(
        [pSocket](uv_loop_t* /*pLoop*/)
        {
            if (!pSocket) return;
            uv_close(
                (uv_handle_t*)pSocket,
                [](uv_handle_t* pHandle)
                {
                    auto pSocket = (uv_tcp_t*)pHandle;
                    delete pSocket;
                }
            );
        }
    );
}

// Creates the UV socket.
// Note: This is not done in the constructor, as that can be called outside
//       the UV loop. It is only called from functions inside the loop.
void Socket::createSocket()
{
    // We create the socket and associate it with the loop.
    // We set its data to point to 'this' so that callbacks can invoke class methods.
    m_pSocket = new uv_tcp_t;
    m_pSocket->data = this;
    uv_tcp_init(
        m_pUVLoop->getUVLoop(),
        m_pSocket
    );
}

// Sets the callback.
void Socket::setCallback(ICallback* pCallback)
{
    m_pCallback = pCallback;
}

// Called when a socket is connected to set up reading and writing.
void Socket::onSocketConnected()
{
    // We disable Nagling...
    uv_tcp_nodelay(m_pSocket, 1);

    // We note the the socket is connected and process any queued writes...
    m_connected = true;
    processQueuedWrites();

    // We start reading data from the socket...
    uv_read_start(
        (uv_stream_t*)m_pSocket,
        UVUtils::allocateBufferMemory,
        [](uv_stream_t* s, ssize_t n, const uv_buf_t* b)
        {
            auto self = (Socket*)s->data;
            self->onDataReceived(s, n, b);
        }
    );
}

// Connects a server socket to listen on the specified port.
void Socket::listen(int port)
{
    // We create a name for the socket from its connection info...
    m_name = Utils::format("LISTENING-SOCKET:%d", port);
    Logger::info("Creating socket: " + m_name);

    // We create the UV socket...
    createSocket();

    // We bind to the specified port on all network interfaces...
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_bind(m_pSocket, (const struct sockaddr*)&addr, 0);

    // We turn of Nagling...
    uv_tcp_nodelay(m_pSocket, 1);

    // We listen for connections, calling onNewConnection() when a connection is received...
    int listenResult = uv_listen(
        (uv_stream_t*)m_pSocket,
        MAX_INCOMING_CONNECTION_BACKLOG,
        [](uv_stream_t* p, int s)
        {
            auto self = (Socket*)p->data;
            self->onNewConnection(p, s);
        }
    );
    if (listenResult)
    {
        Logger::error(Utils::format("uv_listen error: %s", uv_strerror(listenResult)));
    }
}

// Connects the socket by accepting a listen request received by the server.
void Socket::accept(uv_stream_t* pServer)
{
    // We create the UV socket...
    createSocket();

    // We accept the connection...
    if (uv_accept(pServer, (uv_stream_t*)m_pSocket) == 0)
    {
        // We find the name of the client...
        auto peerInfo = UVUtils::getPeerIPInfo(m_pSocket);
        m_name = Utils::format("%s:%s", peerInfo.Hostname.c_str(), peerInfo.Service.c_str());
        Logger::info("Accepted socket: " + m_name);

        // We start reading and writing...
        onSocketConnected();
    }
    else
    {
        Logger::error("Accept error");
        // RSSTODO: Should we close the socket here? Or other notification?
    }
}

// Connects a client socket to the IP address and port specified.
void Socket::connectIP(const std::string& ipAddress, int port)
{
    m_name = Utils::format("%s:%d", ipAddress.c_str(), port);
    Logger::info(Utils::format("Connecting to %s:%d", ipAddress.c_str(), port));

    // We create the UV socket...
    createSocket();

    // We make the connection request...
    struct sockaddr_in destination;
    uv_ip4_addr(ipAddress.c_str(), port, &destination);
    auto pConnect = new uv_connect_t;
    pConnect->data = this;
    uv_tcp_connect(
        pConnect, 
        m_pSocket,
        (const struct sockaddr*)&destination, 
        [](uv_connect_t* r, int s)
        {
            auto self = (Socket*)r->data;
            self->onConnectCompleted(r, s);
        }
    );
}

// Connects a client socket to the hostname and port specified.
void Socket::connect(const std::string& hostname, int port)
{
    try
    {
        Logger::info(Utils::format("Connecting to: %s:%d", hostname.c_str(), port));

        // We create a context to use in callbacks...
        auto pContext = new connect_hostname_t;
        pContext->self = this;
        pContext->port = port;

        // We create a DNS resolution request...
        auto pRequest = new uv_getaddrinfo_t;
        pRequest->data = pContext;

        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;        // IPv4
        hints.ai_socktype = SOCK_STREAM;  // TCP

        auto status = uv_getaddrinfo(
            m_pUVLoop->getUVLoop(),
            pRequest,
            [](uv_getaddrinfo_t* r, int s, struct addrinfo* a)
            {
                auto pContext = (connect_hostname_t*)r->data;
                pContext->self->onDNSResolution(r, s, a);
            },
            hostname.c_str(),
            NULL,
            &hints);
        if (status)
        {
            // An error has occurred...
            auto error = uv_strerror(status);
            Logger::error(Utils::format("Hostname resolution error: %s", error));
            delete pContext;
            delete pRequest;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when DNS resolution has completed for a hostname.
void Socket::onDNSResolution(uv_getaddrinfo_t* pRequest, int status, struct addrinfo* pAddressInfo)
{
    try
    {
        auto pContext = (connect_hostname_t*)pRequest->data;
        auto port = pContext->port;

        // We check if the resolution was successful...
        if (status < 0) 
        {
            // An error occurred...
            auto error = uv_strerror(status);
            Logger::error(Utils::format("Hostname resolution error: %s", error));
            delete pContext;
            delete pRequest;
            return;
        }

        // We get the first resolved IP address and connect to it...
        char ipAddress[17] = { '\0' };
        uv_ip4_name((struct sockaddr_in*)pAddressInfo->ai_addr, ipAddress, 16);
        auto strIPAddress = std::string(ipAddress);
        connectIP(strIPAddress, port);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called at the client side when a client has connected to a server.
void Socket::onConnectCompleted(uv_connect_t* pRequest, int status)
{
    try
    {
        delete pRequest;
        if (status < 0)
        {
            Logger::error(Utils::format("Connection error: %s", uv_strerror(status)));
            return;
        }

        // We start reading and writing...
        onSocketConnected();
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Moves the socket to be managed by the UV loop specified.
void Socket::moveToLoop(UVLoopPtr pLoop)
{
    // To move a socket to a new loop we:
    // - Create a duplicate socket
    // - Mark the socket as not connected (so that writes are queued)
    // - Close the original UV socket handle
    // - Wait for close to complete
    // - Change the Socket's UVLoop to the new one
    // - Register the duplicated socket on the new loop

    Logger::info("Moving socket to loop: " + pLoop->getName());

    // We duplicate the socket...
    auto pNewOSSocket = UVUtils::duplicateSocket(m_pSocket->socket);

    // We mark the socket as not connected...
    m_connected = false;

    // We close the socket...
    auto pMoveInfo = new move_socket_t;
    pMoveInfo->self = this;
    pMoveInfo->pNewOSSocket = pNewOSSocket;
    pMoveInfo->pNewUVLoop = pLoop;
    m_pSocket->data = pMoveInfo;
    uv_close(
        (uv_handle_t*)m_pSocket,
        [](uv_handle_t* pHandle)
        {
            // The move continues in moveToLoop_onSocketClosed()...
            auto pMoveInfo = (move_socket_t*)pHandle->data;
            pMoveInfo->self->moveToLoop_onSocketClosed(pMoveInfo);
        }
    );
}

// Called after the original socket is closed as part of moving the socket to another UV loop.
void Socket::moveToLoop_onSocketClosed(move_socket_t* pMoveInfo)
{
    try
    {
        // We delete the original UV socket handle...
        delete m_pSocket;
        m_pSocket = nullptr;

        // We are currently still running in the original UV loop.
        // We marshall the registration of the new (duplicated) socket to the new loop...
        auto pNewUVLoop = pMoveInfo->pNewUVLoop;
        auto pNewOSSocket = pMoveInfo->pNewOSSocket;
        pMoveInfo->pNewUVLoop->marshallEvent(
            [this, pNewUVLoop, pNewOSSocket](uv_loop_t* /*pLoop*/)
            {
                // The move continues in moveToLoop_registerDuplicatedSocket()...
                moveToLoop_registerDuplicatedSocket(pNewUVLoop, pNewOSSocket);
            }
        );

        // We clean up the move-info...
        delete pMoveInfo;
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Connects to the (duplicated) socket specified.
// Note: This is called on the thread for the UVLoop to which we are moving the socket.
void Socket::moveToLoop_registerDuplicatedSocket(UVLoopPtr pUVLoop, OSSocketHolderPtr pOSSocket)
{
    try
    {
        auto socket = pOSSocket->getSocket();
        Logger::info(Utils::format("Connecting to duplicated socket %d", socket));

        // We switch to the new UV loop...
        m_pUVLoop = pUVLoop;

        // We create the UV socket...
        createSocket();

        // We open the socket, connecting to the socket passed in...
        auto status = uv_tcp_open(m_pSocket, socket);
        if (status != 0)
        {
            Logger::error(Utils::format("uv_tcp_open failed: %s", uv_strerror(status)));
            return;
        }

        // We start reading and writing...
        onSocketConnected();

        // We notify that the move to the new loop has completed...
        if (m_pCallback) m_pCallback->onMoveToLoopComplete(this);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Queues data to be written to the socket.
// Can be called from any thread, not just from the uv loop thread.
// Queued writes will be coalesced into one network update.
// RSSTODO: We need some way to slow down the client if it publishes too much too fast.
void Socket::write(BufferPtr pBuffer)
{
    // We queue the data to write...
    m_queuedWrites.add(pBuffer);

    // We marshall an event to write the data. As this does not take place straight 
    // away, this allows us to coalesce multiple queued writes...
    m_pUVLoop->marshallUniqueEvent(
        UVLoop::UniqueEventKey::SOCKET_QUEUE_WRITE,
        [this](uv_loop_t* /*pLoop*/)
        {
            processQueuedWrites();
        }
    );
}

// Sends a coalesced network message for all queued writes.
void Socket::processQueuedWrites()
{
    try
    {
        // We check if the socket is connected...
        if (!m_connected)
        {
            return;
        }

        // We find the combined size of the queued writes...
        auto queuedWrites = m_queuedWrites.getItems();
        size_t totalSize = 0;
        for (auto queuedWrite : *queuedWrites)
        {
            totalSize += queuedWrite->getBufferSize();
        }

        // We create a write-request with a buffer to hold all the queued items...
        auto pWriteRequest = UVUtils::allocateWriteRequest(totalSize);
        pWriteRequest->write_request.data = this;

        // We copy the data into the buffer...
        size_t bufferPosition = 0;
        for (auto queuedWrite : *queuedWrites)
        {
            auto itemSize = queuedWrite->getBufferSize();
            auto itemData = queuedWrite->getBuffer();
            std::memcpy(pWriteRequest->buffer.base + bufferPosition, itemData, itemSize);
            bufferPosition += itemSize;
        }

        // We write the combined buffer...
        uv_write(
            &pWriteRequest->write_request,
            (uv_stream_t*)m_pSocket,
            &pWriteRequest->buffer,
            1,
            [](uv_write_t* r, int s)
            {
                auto self = (Socket*)r->data;
                self->onWriteCompleted(r, s);
            }
        );
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when a write request has completed.
void Socket::onWriteCompleted(uv_write_t* pRequest, int status)
{
    try
    {
        // We check the status...
        if (status < 0)
        {
            Logger::error(Utils::format("Write error: %s", uv_strerror(status)));
        }

        // We release the write request (including the buffer)...
        auto pWriteRequest = (UVUtils::WriteRequest*)pRequest;
        UVUtils::releaseWriteRequest(pWriteRequest);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called at the server side when a new client connection has been received
// on a listening socket.
void Socket::onNewConnection(uv_stream_t* pServer, int status)
{
    try
    {
        // We check that the connection has succeeded...
        Logger::info("onNewConnection");
        if (status < 0)
        {
            Logger::error(Utils::format("Connection error: %s", uv_strerror(status)));
            return;
        }

        // We create a client socket for the new connection...
        auto clientSocket = Socket::create(m_pUVLoop);
        clientSocket->accept(pServer);

        // We pass the socket to the callback...
        if (m_pCallback) m_pCallback->onNewConnection(clientSocket);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Called when data has been received on a socket.
void Socket::onDataReceived(uv_stream_t* /*pStream*/, ssize_t nread, const uv_buf_t* pBuffer)
{
    try
    {
        // We check for errors...
        if (nread < 1)
        {
            auto error = uv_strerror((int)nread);
            Logger::info(Utils::format("onDataReceived: %s", error));
            if (nread == UV_EOF || nread == UV_ECONNRESET)
            {
                if (m_pCallback) m_pCallback->onDisconnected(this);
            }
            return;
        }

        // All data we receive comes in the form of network-data messages.
        // These look like:
        // - size  (int32, little-endian)
        // - bytes[size]
        //
        // There are different cases where we can get this callback:
        // 
        // 1. A data buffer containing all (and only) the data for one message.
        //
        // 2. A data buffer containing multiple messages.
        // 
        // 3. Partial data for a message. In particular this will be the case
        //    for very large messages which do not fit into one data buffer.
        //
        // It could be that the buffer holds data for some combination of the
        // types described above. For example:
        // - Two complete messages followed by one partial message.
        // - The remainder of a message from a previous update, plus one or more new messages.
        // - Other permutations and combinations along these lines.
        // 
        // In all cases we only really need to know two things:
        // 
        // a) Are we expecting a new message? 
        //    If so, we read the size and then read that number of bytes.
        // 
        // b) Are we in the process of reading a message?
        //    We have a message that started in a previous update and has data
        //    continuing into this one.
        // 
        // We are always reading one message at a time into the m_currentMessage object.
        // This is a shared pointer to a NetworkData object.
        // - If this is null we are expecting a new message and need to read the size. 
        // - If this is not null we are reading additional data for an existing message.
        // 
        // The NetworkData object knows the size we are expecting, so we can continue to
        // read data until we have all the data we need for the message.
        // 
        // NOTE: We need to be careful when reading the size for a new message. It is
        //       possible that the size itself may only be received across multiple of
        //       these callbacks.

        // We read the buffer...
        size_t bufferSize = nread;
        size_t bufferPosition = 0;
        while (bufferPosition < bufferSize)
        {
            // If we do not have a current message we create one...
            if (!m_pCurrentMessage)
            {
                m_pCurrentMessage = Buffer::create();
            }

            // We read data into the current message...
            size_t bytesRead = m_pCurrentMessage->readNetworkMessage(pBuffer->base, bufferSize, bufferPosition);

            // If we have read all data for the current message we call back with it...
            if (m_pCurrentMessage->hasAllData())
            {
                // We reset the position of the message / buffer so that it is 
                // ready to be read by the client in the callback...
                m_pCurrentMessage->resetPosition();
                if (m_pCallback) m_pCallback->onDataReceived(this, m_pCurrentMessage);

                // We clear the current message to start a new one...
                m_pCurrentMessage = nullptr;
            }

            // We update the buffer position and loop to check if there is
            // more data to read...
            bufferPosition += bytesRead;
        }

        // We release the buffer memory...
        UVUtils::releaseBufferMemory(pBuffer);
    }
    catch (const std::exception& ex)
    {
        Logger::error(Utils::format("%s: %s", __func__, ex.what()));
    }
}

