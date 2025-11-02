#pragma once
#include <functional>
#include <string>
#include "SharedPointers.h"

namespace MessagingMesh
{
    // Signature for subscription callbacks.
    typedef std::function<void(const std::string& subject, const std::string& replySubject, MessagePtr pMessage)> SubscriptionCallback;
} // namespace


