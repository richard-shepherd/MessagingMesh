#pragma once
#include <memory>

namespace MessagingMesh
{
    // Shared pointer to a BLOB.
    class BLOB;
    using BLOBPtr = std::shared_ptr<BLOB>;
    using ConstBLOBPtr = std::shared_ptr<const BLOB>;

    // Shared pointer to a Field.
    class Field;
    using FieldPtr = std::shared_ptr<Field>;
    using ConstFieldPtr = std::shared_ptr<const Field>;

    // Shared pointer to a Message.
    class Message;
    using MessagePtr = std::shared_ptr<Message>;
    using ConstMessagePtr = std::shared_ptr<const Message>;

    // Shared pointer to a Buffer.
    class Buffer;
    using BufferPtr = std::shared_ptr<Buffer>;
    using ConstBufferPtr = std::shared_ptr<const Buffer>;

    // Shared pointer to a Socket.
    class Socket;
    using SocketPtr = std::shared_ptr<Socket>;

    // Shared pointer to an OSSocketHolder.
    class OSSocketHolder;
    using OSSocketHolderPtr = std::shared_ptr<OSSocketHolder>;

    // Shared pointer to a UVLoop.
    class UVLoop;
    using UVLoopPtr = std::shared_ptr<UVLoop>;

    // Shared pointer to a Subscription.
    class Subscription;
    using SubscriptionPtr = std::shared_ptr<Subscription>;

} // namespace
