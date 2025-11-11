#pragma once
#include <memory>

namespace MessagingMesh
{
    // Forward declarations...
    class Field;
    class Message;
    class Buffer;
    class Socket;
    class OSSocketHolder;
    class UVLoop;
    class Subscription;
    class AutoResetEvent;

    // Shared pointer to a Field.
    using FieldPtr = std::shared_ptr<Field>;

    // Shared pointer to a const Field.
    using ConstFieldPtr = std::shared_ptr<const Field>;

    // Shared pointer to a Message.
    using MessagePtr = std::shared_ptr<Message>;

    // Shared pointer to a const Message.
    using ConstMessagePtr = std::shared_ptr<const Message>;

    // Shared pointer to a Buffer.
    using BufferPtr = std::shared_ptr<Buffer>;

    // Shared pointer to a const Buffer.
    using ConstBufferPtr = std::shared_ptr<const Buffer>;

    // Shared pointer to a Socket.
    using SocketPtr = std::shared_ptr<Socket>;

    // Shared pointer to an OSSocketHolder.
    using OSSocketHolderPtr = std::shared_ptr<OSSocketHolder>;

    // Shared pointer to a UVLoop.
    using UVLoopPtr = std::shared_ptr<UVLoop>;

    // Shared pointer to a Subscription.
    using SubscriptionPtr = std::shared_ptr<Subscription>;

    // Shared pointer to an auot reset event.
    using AutoResetEventPtr = std::shared_ptr<AutoResetEvent>;

} // namespace
