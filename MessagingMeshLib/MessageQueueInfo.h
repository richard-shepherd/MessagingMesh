#pragma once

namespace MessagingMesh
{
    // Returned from Connection::processMessageQueue() with info about messages processed and the queue size.
    struct MessageQueueInfo
    {
        // The number of messages processed in processMessageQueue().
        size_t MessagesProcessed = 0;

        // The number of queued messages remaining to be processed.
        size_t QueueSize = 0;
    };
} // namespace

