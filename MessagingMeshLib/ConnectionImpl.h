#pragma once
#include <string>
#include <map>
#include <atomic>
#include "SharedPointers.h"
#include "Socket.h"
#include "AutoResetEvent.h"
#include "Callbacks.h"

namespace MessagingMesh
{
    // Forward declarations...
    class NetworkMessage;
    class NetworkMessageHeader;

    /// <summary>
    /// Implementation of the Connection class, ie a client connection
    /// to the messaging-mesh.
    /// </summary>
    class ConnectionImpl : Socket::ICallback
    {
    // Public methods...
    public:
        // Constructor.
        ConnectionImpl(const std::string& hostname, int port, const std::string& service);

        // Destructor.
        ~ConnectionImpl();

        // Sends a message to the specified subject.
        void sendMessage(const std::string& subject, const MessagePtr& pMessage) const;

        // Subscribes to a subject.
        // The lifetime of the subscription is the lifetime of the object returned.
        SubscriptionPtr subscribe(const std::string& subject, SubscriptionCallback callback);

        // Unsubscribes from a subscription.
        void unsubscribe(uint32_t subscriptionID, bool removeFromCollection);

    // Socket::ICallback implementation
    private:
        // Called when a new client connection has been made to a listening socket.
        // Called on the UV loop thread.
        void onNewConnection(SocketPtr /*pClientSocket*/) {}

        // Called when data has been received on the socket.
        // Called on the UV loop thread.
        void onDataReceived(Socket* pSocket, BufferPtr pBuffer);

        // Called when a socket has been disconnected.
        void onDisconnected(Socket* pSocket);

        // Called when the movement of the socket to a new UV loop has been completed.
        void onMoveToLoopComplete(Socket* /*pSocket*/) {}

    // Private functions...
    private:
        // Called when we see the ACK message from the Gateway.
        void onAck();

        // Called when we see the SEND_MESSAGE message from the Gateway.
        void onSendMessage(const NetworkMessageHeader& header, BufferPtr pBuffer);

    // Private data...
    private:
        // Construction params...
        std::string m_hostname;
        int m_port;
        std::string m_service;

        // UV loop for client messaging...
        UVLoopPtr m_pUVLoop;

        // Socket connection to the gateway...
        SocketPtr m_pSocket;

        // Waits for the ACK signal...
        AutoResetEvent m_ackSignal;

        // Threadsafe subscription ID.
        // Note: This starts at 1, which means we can use 0 to indicate an invalid subscription ID.
        std::atomic<uint32_t> m_nextSubscriptionID = 1;

        // Active subscriptions, keyed by subscription ID.
        // Note: This holds non-shared pointers as the lifetime of Subscriptions objects
        //       is managed by the shared-pointers passed to client code.
        // RSSTODO: THIS NEEDS TO BE THREAD SAFE!!!
        std::map<uint32_t, Subscription*> m_subscriptions;
    };
} // namespace


