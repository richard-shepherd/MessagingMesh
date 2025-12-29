#pragma once
#include <functional>
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class Connection;

    // Enum passed with notification callbacks.
    enum class NotificationType
    {
        // Notification sent when the connection has been completed (in particular for asynchronous connection).
        CONNECTED
    };

    // Signature for subscription callbacks.
    using SubscriptionCallback = std::function<void(Connection& connection, const std::string& subject, const std::string& replySubject, MessagePtr pMessage, void* tag)>;

    // Signature for notification callbacks.
    using NotificationCallback = std::function<void(Connection& connection, NotificationType notificationType, const std::string& info)>;

} // namespace


