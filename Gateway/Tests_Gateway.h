#pragma once
#include <cstdint>
#include <TestUtils.h>
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
        // Runs all tests.
        static void runAll();

        // Tests for the subject-matching engine.
        static void subjectMatchingEngine(TestUtils::TestRun& testRun);

    // Private functions...
    private:
        // Returns the subscription ID (as an int) if the collection contains it, -1 if not.
        static int containsID(const VecSubscriptionInfo& subscriptionInfos, uint32_t subscriptionID);
    };
}  // namespace


