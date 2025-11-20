#pragma once
#include <functional>
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Forward declarations...
    class Connection;

    // Signature for subscription callbacks.
    using SubscriptionCallback = std::function<void(Connection& connection, const std::string& subject, const std::string& replySubject, MessagePtr pMessage, void* tag)>;

} // namespace


