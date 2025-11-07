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
        // Returns true of the subscription-infos include one for the subscriptionID specified, false if not.
        static bool containsSubscriptionID(const VecSubscriptionInfo& subscriptionInfos, uint32_t subscriptionID);
    };
}  // namespace


