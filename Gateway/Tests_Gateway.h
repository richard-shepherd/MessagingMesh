#pragma once
#include <cstdint>
#include "GatewaySharedPointers.h"

namespace MessagingMesh
{
    /// <summary>
    /// Tests for the Gateway.
    /// </summary>
    class Tests_Gateway
    {
    // Public methods...
    public:
        // Test for the subject-matching engine.
        static void subjectMatchingEngine();

    // Private functions...
    private:
        // Returns the subscription ID (as an int) is the collection conatins it, -1 if not.
        static int containsID(const VecSubscriptionInfo& subscriptionInfos, uint32_t subscriptionID);
    };
}  // namespace


