#include "Socket.h"
#include <format>
#include "Logger.h"
#include "UVUtils.h"
#include "UVLoop.h"
#include "Buffer.h"
#include "OSSocketHolder.h"
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
    // We create a unique ID (within this process) for the socket and a write event key.
    // The key helps us marshal write events between the write() to processQueuedWrites() methods.
    m_socketID = ++m_atomicSocketID;
    m_writeEventKey = std::format("S{}W", m_socketID);
}

// Destructor.
Socket::~Socket()
{
    Logger::info(std::format("Closing socket: {}", m_name));

    // We mark the socket as not connected and clear the write queue...
    m_connected = false;
    m_queuedWrites.clear();

    // The destructor could be called from a different thread than the one running the UV loop, so we marshall 
    // the socket close event to the socket's UV loop. (Provided that the socket has not already been closed.)
    if (m_pSocket)
    {
        auto pSocket = (uv_handle_t*)m_pSocket;
        m_pUVLoop->marshallEvent(
            [pSocket](uv_loop_t* /*pLoop*/)
            {
                // We close the socket (if it is not already being closed)...
                if (!uv_is_closing(pSocket))
                {
                    uv_close(pSocket, on_uv_close_callback);
                }
            }
        );
    }
}

// (Static) callback from uv_close.
void Socket::on_uv_close_callback(uv_handle_t* handle)
{
    // We delete the UV socket handle...
    auto pSocket = (uv_tcp_t*)handle;
    delete pSocket;
}

// Sets the client ID (used as part of the socket name).
void Socket::setClientID(const std::string& clientID)
{
    // We update the name to include the the client ID...
    m_name = std::format("[{}]:[{}]", clientID, m_name);
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
    uv_read_start((uv_stream_t*)m_pSocket, UVUtils::allocateBufferMemory, on_uv_read_start_callback);
}

// (Static) callback from uv_read_start.
void Socket::on_uv_read_start_callback(uv_stream_t* stream, ssize_t n, const uv_buf_t* buffer)
{
    // TODO-SOCKET: Use of this / self
    auto self = (Socket*)stream->data;
    self->onDataReceived(stream, n, buffer);
}

// Connects a server socket to listen on the specified port.
void Socket::listen(int port)
{
    // We create a name for the socket from its connection info...
    m_name = std::format("LISTENING-SOCKET:{}", port);
    Logger::info("Creating socket: " + m_name);

    // We create the UV socket...
    createSocket();

    // We bind to the specified port on all network interfaces...
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_bind(m_pSocket, (const struct sockaddr*)&addr, 0);

    // We turn off Nagling...
    uv_tcp_nodelay(m_pSocket, 1);

    // We listen for connections, calling onNewConnection() when a connection is received...
    int listenResult = uv_listen((uv_stream_t*)m_pSocket, MAX_INCOMING_CONNECTION_BACKLOG, on_uv_listen_callback);
    if (listenResult)
    {
        Logger::error(std::format("uv_listen error: {}", uv_strerror(listenResult)));
    }
}

// (Static) callback from uv_listen.
void Socket::on_uv_listen_callback(uv_stream_t* stream, int status)
{
    // TODO-SOCKET: Use of this / self
    auto self = (Socket*)stream->data;
    self->onNewConnection(stream, status);
}

// Connects the socket by accepting a listen request received by the server.
void Socket::accept(uv_stream_t* pServer)
{
    // We create the UV socket...
    createSocket();

    // We accept the connection...
    if (uv_accept(pServer, (uv_stream_t*)m_pSocket) == 0)
    {
        // We set buffer sizes...
        int size = 1024 * 1024; // 1MB
        uv_send_buffer_size((uv_handle_t*)m_pSocket, &size);
        uv_recv_buffer_size((uv_handle_t*)m_pSocket, &size);

        // We find the name of the client...
        auto peerInfo = UVUtils::getPeerIPInfo(m_pSocket);
        m_name = std::format("{}:{}", peerInfo.Hostname, peerInfo.Service);
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
    // If the name has not already been set we create it.
    // (It could be set already if the connection was requested by (hostname, port) which then resolved to an IP address.)
    if (m_name.empty())
    {
        m_name = std::format("{}:{}", ipAddress, port);
        Logger::info(std::format("Connecting to {}", m_name));
    }

    // We create the UV socket...
    createSocket();

    // We make the connection request...
    struct sockaddr_in destination;
    uv_ip4_addr(ipAddress.c_str(), port, &destination);
    auto pConnect = new uv_connect_t;
    pConnect->data = this;
    uv_tcp_connect(pConnect, m_pSocket, (const struct sockaddr*)&destination, on_uv_tcp_connect_callback);
}

// (Static) callback from uv_tcp_connect.
void Socket::on_uv_tcp_connect_callback(uv_connect_t* request, int status)
{
    // TODO-SOCKET: Use of this / self
    auto self = (Socket*)request->data;
    self->onConnectCompleted(request, status);
}

// Connects a client socket to the hostname and port specified.
void Socket::connect(const std::string& hostname, int port)
{
    try
    {
        m_name = std::format("{}:{}", hostname, port);
        Logger::info(std::format("Connecting to: {}", m_name));

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

        auto status = uv_getaddrinfo(m_pUVLoop->getUVLoop(), pRequest, on_uv_getaddrinfo_callback, hostname.c_str(), NULL, &hints);
        if (status)
        {
            // An error has occurred...
            auto error = uv_strerror(status);
            Logger::error(std::format("Hostname resolution error: {}", error));
            delete pContext;
            delete pRequest;
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// (Static) callback from uv_getaddrinfo.
void Socket::on_uv_getaddrinfo_callback(uv_getaddrinfo_t* request, int status, struct addrinfo* address_info)
{
    // TODO-SOCKET: Use of this / self
    auto pContext = (connect_hostname_t*)request->data;
    pContext->self->onDNSResolution(request, status, address_info);
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
            Logger::error(std::format("Hostname resolution error: {}", error));
            delete pContext;
            delete pRequest;
            return;
        }

        // We get the first resolved IP address and connect to it...
        char ipAddress[17] = { '\0' };
        uv_ip4_name((struct sockaddr_in*)pAddressInfo->ai_addr, ipAddress, 16);
        auto strIPAddress = std::string(ipAddress);
        connectIP(strIPAddress, port);

        // We free the address info...
        uv_freeaddrinfo(pAddressInfo);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
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
            if (m_pCallback)
            {
                m_pCallback->onConnectionStatusChanged(this, ConnectionStatus::CONNECTION_FAILED, uv_strerror(status));
            }
            return;
        }

        // The connection succeeded, so we start reading and writing...
        onSocketConnected();

        // We notify that the socket is connected.
        // NOTE: This must be done after the line above to ensure that actions taken by the callback 
        //       on the socket happen after it is fully connected.
        if (m_pCallback)
        {
            m_pCallback->onConnectionStatusChanged(this, ConnectionStatus::CONNECTION_SUCCEEDED, "");
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
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
    uv_close((uv_handle_t*)m_pSocket, on_uv_close_move_socket_callback);
}

// (Static) callback from uv_close (when used to move a socket to a new loop).
void Socket::on_uv_close_move_socket_callback(uv_handle_t* handle)
{
    // TODO-SOCKET: Use of this / self
    // The move continues in moveToLoop_onSocketClosed()...
    auto pMoveInfo = (move_socket_t*)handle->data;
    pMoveInfo->self->moveToLoop_onSocketClosed(pMoveInfo);
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
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Connects to the (duplicated) socket specified.
// Note: This is called on the thread for the UVLoop to which we are moving the socket.
void Socket::moveToLoop_registerDuplicatedSocket(UVLoopPtr pUVLoop, OSSocketHolderPtr pOSSocket)
{
    try
    {
        auto socket = pOSSocket->getSocket();

        // We switch to the new UV loop...
        m_pUVLoop = pUVLoop;

        // We create the UV socket...
        createSocket();

        // We open the socket, connecting to the socket passed in...
        auto status = uv_tcp_open(m_pSocket, socket);
        if (status != 0)
        {
            Logger::error(std::format("uv_tcp_open failed: {}", uv_strerror(status)));
            return;
        }

        // We start reading and writing...
        onSocketConnected();

        // We notify that the move to the new loop has completed...
        if (m_pCallback)
        {
            m_pCallback->onMoveToLoopComplete(this);
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Queues data to be written to the socket.
// Can be called from any thread, not just from the uv loop thread.
// Queued writes will be coalesced into one network update.
// RSSTODO: We need some way to slow down the client if it publishes too much too fast.
void Socket::write(BufferPtr pBuffer, uint32_t subscriptionIDOverride)
{
    // We queue the data to write...
    BufferInfo bufferInfo(pBuffer, subscriptionIDOverride);
    m_queuedWrites.add(bufferInfo);

    // We take a shared pointer to the socket. This keeps it alive until the marshalled
    // event (below) has occurred.
    auto self = shared_from_this();

    // We marshall an event to write the data. As this does not take place straight 
    // away, this allows us to coalesce multiple queued writes...
    m_pUVLoop->marshallUniqueEvent(
        m_writeEventKey,
        [self](uv_loop_t* /*pLoop*/)
        {
            self->processQueuedWrites();
        }
    );
}

// Sends network messages for all queued writes.
void Socket::processQueuedWrites()
{
    try
    {
        // We check if the socket is connected...
        if (!m_connected)
        {
            return;
        }

        // We convert the queued writes into UV write-requests and send them...
        auto queuedWrites = m_queuedWrites.getItems();
        getWriteRequests(*queuedWrites, [&](UVUtils::WriteRequest* pWriteRequest)
            {
                send(pWriteRequest);
            });
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Sends data to the socket.
void Socket::send(UVUtils::WriteRequest* pWriteRequest)
{
    uv_write(&pWriteRequest->write_request, (uv_stream_t*)m_pSocket, &pWriteRequest->buffer, 1, on_uv_write_callback);
}

// (Static) callback from uv_write.
void Socket::on_uv_write_callback(uv_write_t* request, int status)
{
    auto pWriteRequest = (UVUtils::WriteRequest*)request;
    pWriteRequest->pSocket->onWriteCompleted(request, status);
}

// Calls back with UV write requests to send for the queued data.
void Socket::getWriteRequests(const std::vector<BufferInfo>& bufferInfos, std::function<void(UVUtils::WriteRequest*)> callback)
{
    // How we fill in the write requests depends on the size of the buffers in the queue.
    // 
    // Small buffers
    // -------------
    // If we have a lot of small buffers it is more efficient to combine them into one
    // larger buffer before sending, to reduce the number of sends on the socket.
    //
    // Large buffers
    // -------------
    // For large buffers it is better to send them in one go.
    //
    // What we do
    // ----------
    // - We have pSmallMessagesWriteRequest for aggregating smaller messages. 
    //
    // - If a buffer is small (<= the small message send buffer size) we add it to the
    //   small message write request. If it does not fit, we add what we can, and add the remainder
    //   to the next small message write request.
    //   
    // - If a buffer is large (> small message send buffer size) then we send the buffer as it is.
    //   NOTE: We make sure that we have sent any partially filled small write request first.
    //
    // Subscription ID override
    // ------------------------
    // When sending messages from the Gateway to clients buffer-infos may have a subscription ID
    // override. This is the client-specific subscription ID for its subscription to the message.
    // We need to make sure that this is 'pasted-in' to the appropriate position in the data we 
    // send, ie just after the Size at the start of the buffer.

    // Size in a buffer of the Size + Subscription ID...
    const int SIZE_PLUS_SUBSCRIPTION_ID = Buffer::SIZE_SIZE + sizeof(uint32_t);

    // The small buffer write request...
    const int SMALL_MESSAGE_SEND_BUFFER_SIZE = 8192;
    UVUtils::WriteRequest* pSmallMessagesWriteRequest = nullptr;
    auto smallMessageSendBufferPosition = 0;

    // We process each buffer...
    for(const auto& bufferInfo : bufferInfos)
    {
        // We check if the buffer is small or large...
        auto bufferSize = bufferInfo.pBuffer->getBufferSize();
        auto bufferData = bufferInfo.pBuffer->getBuffer();
        if (bufferSize < SIZE_PLUS_SUBSCRIPTION_ID)
        {
            // This does not look like a Messaging Mesh buffer.
            continue;
        }
        if (bufferSize <= SMALL_MESSAGE_SEND_BUFFER_SIZE)
        {
            // We have a small message, so we add it to the small message write request...
            auto bufferPosition = 0;
            while (bufferPosition < bufferSize)
            {
                // We add as much as we can to the small message send buffer.
                auto sizeRemaining_smallMessageSendBuffer = SMALL_MESSAGE_SEND_BUFFER_SIZE - smallMessageSendBufferPosition;
                auto sizeRemaining_buffer = bufferSize - bufferPosition;
                auto sizeToCopy = std::min(sizeRemaining_buffer, sizeRemaining_smallMessageSendBuffer);

                // If we are writing from the start of the buffer we want the size-to-copy to be at least 
                // eight bytes (for the Size and Subscription ID). If we do not have enough space we send
                // the small message write request and create a new one...
                if (bufferPosition == 0 && sizeToCopy < SIZE_PLUS_SUBSCRIPTION_ID && pSmallMessagesWriteRequest != nullptr)
                {
                    pSmallMessagesWriteRequest->buffer.len = smallMessageSendBufferPosition;
                    callback(pSmallMessagesWriteRequest);
                    pSmallMessagesWriteRequest = nullptr;
                    smallMessageSendBufferPosition = 0;
                    continue;
                }
                // We copy the data to the write request...
                if (pSmallMessagesWriteRequest == nullptr)
                {
                    pSmallMessagesWriteRequest = UVUtils::allocateWriteRequest(SMALL_MESSAGE_SEND_BUFFER_SIZE, shared_from_this());
                }
                std::memcpy(pSmallMessagesWriteRequest->buffer.base + smallMessageSendBufferPosition, bufferData + bufferPosition, sizeToCopy);

                // If we are writing the first part of the buffer and we have an overridden subscription ID
                // we override it in the write request...
                if (bufferPosition == 0 && bufferInfo.subscriptionIDOverride != 0)
                {
                    std::memcpy(pSmallMessagesWriteRequest->buffer.base + smallMessageSendBufferPosition + Buffer::SIZE_SIZE, &bufferInfo.subscriptionIDOverride, sizeof(uint32_t));
                }

                // We update the buffer positions...
                smallMessageSendBufferPosition += sizeToCopy;
                bufferPosition += sizeToCopy;

                // If we have filled the small message buffer we send it...
                if (smallMessageSendBufferPosition == SMALL_MESSAGE_SEND_BUFFER_SIZE)
                {
                    pSmallMessagesWriteRequest->buffer.len = smallMessageSendBufferPosition;
                    callback(pSmallMessagesWriteRequest);
                    pSmallMessagesWriteRequest = nullptr;
                    smallMessageSendBufferPosition = 0;
                }
            }
        }
        else
        {
            // We have a large message.
            // We make sure that any partially filled small buffer has been sent.
            if (pSmallMessagesWriteRequest != nullptr)
            {
                pSmallMessagesWriteRequest->buffer.len = smallMessageSendBufferPosition;
                callback(pSmallMessagesWriteRequest);
                pSmallMessagesWriteRequest = nullptr;
                smallMessageSendBufferPosition = 0;
            }

            // We send the large buffer...
            auto pLargeWriteRequest = UVUtils::allocateWriteRequest(bufferSize, shared_from_this());
            std::memcpy(pLargeWriteRequest->buffer.base, bufferData, bufferSize);

            // We override the subscription ID if needed...
            if (bufferInfo.subscriptionIDOverride != 0)
            {
                std::memcpy(pLargeWriteRequest->buffer.base + Buffer::SIZE_SIZE, &bufferInfo.subscriptionIDOverride, sizeof(uint32_t));
            }

            callback(pLargeWriteRequest);
        }
    }

    // We send any remaining small message buffer...
    if (pSmallMessagesWriteRequest != nullptr)
    {
        pSmallMessagesWriteRequest->buffer.len = smallMessageSendBufferPosition;
        callback(pSmallMessagesWriteRequest);
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
            // We log the error...
            auto error = std::string(uv_strerror(status));
            Logger::error(std::format("Write error on {}: {}", m_name, error));

            // It looks like the socket has disconnected...
            handleSocketDisconnected(error);
        }

        // We release the write request (including the buffer)...
        auto pWriteRequest = (UVUtils::WriteRequest*)pRequest;
        UVUtils::releaseWriteRequest(pWriteRequest);
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when the socket has disconnected, to reset the connection and notify observers.
void Socket::handleSocketDisconnected(const std::string& error)
{
    Logger::warn(std::format("Socket disconnected {}: {}", m_name, error));

    // We note that the socket is no longer connected...
    m_connected = false;

    // We clear the write queue...
    m_queuedWrites.clear();

    // We notify observers...
    if (m_pCallback)
    {
        m_pCallback->onConnectionStatusChanged(this, ConnectionStatus::DISCONNECTED, error);
    }
}

// Called at the server side when a new client connection has been received
// on a listening socket.
void Socket::onNewConnection(uv_stream_t* pServer, int status)
{
    try
    {
        // We check that the connection has succeeded...
        if (status < 0)
        {
            Logger::error(std::format("Connection error: {}", uv_strerror(status)));
            return;
        }

        // We create a client socket for the new connection...
        auto clientSocket = Socket::create(m_pUVLoop);
        clientSocket->accept(pServer);

        // We pass the socket to the callback...
        if (m_pCallback)
        {
            m_pCallback->onNewConnection(clientSocket);
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Called when data has been received on a socket.
void Socket::onDataReceived(uv_stream_t* /*pStream*/, ssize_t nread, const uv_buf_t* pBuffer)
{
    try
    {
        // If we have (somehow) received zero bytes we free the buffer and return...
        if (nread == 0)
        {
            UVUtils::releaseBufferMemory(pBuffer);
            return;
        }

        // We check for errors...
        if (nread < 0)
        {
            // We free the buffer...
            UVUtils::releaseBufferMemory(pBuffer);

            // We log the error, notify the callback and return...
            auto error = std::string(uv_strerror((int)nread));
            Logger::warn(std::format("onDataReceived from {}: {}", m_name, error));
            handleSocketDisconnected(error);
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
                if (m_pCallback)
                {
                    m_pCallback->onDataReceived(this, m_pCurrentMessage);
                }

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
        Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

