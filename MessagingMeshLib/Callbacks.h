#pragma once
#include <functional>
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Signature for subscription callbacks.
    using SubscriptionCallback = std::function<void(const std::string& subject, const std::string& replySubject, MessagePtr pMessage)>;

} // namespace


