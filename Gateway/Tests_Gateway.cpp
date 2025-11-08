#include "Tests_Gateway.h"
#include "SubjectMatchingEngine.h"
#include "SubscriptionInfo.h"
#include "TestUtils.h"
using namespace MessagingMesh;
using namespace MessagingMesh::TestUtils;

// Test for the subject-matching engine.
void Tests_Gateway::subjectMatchingEngine()
{
    TestUtils::log("No wildcards...");
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);
        sme.addSubscription("A.B.D", 345, "ClientC", nullptr);
        sme.addSubscription("A.B.D.E", 456, "ClientC", nullptr);
        sme.addSubscription("A.B.D.E.F", 567, "ClientC", nullptr);

        // We check for matches...
        auto matchesABC = sme.getMatchingSubscriptionInfos("A.B.C");
        assertEqual(matchesABC.size(), (size_t)2);
        assertEqual(matchesABC[0]->getSubscriptionID(), (uint32_t)123);
        assertEqual(matchesABC[1]->getSubscriptionID(), (uint32_t)234);

        // We check for matches...
        auto matchesABD = sme.getMatchingSubscriptionInfos("A.B.D");
        assertEqual(matchesABD.size(), (size_t)1);
        assertEqual(matchesABD[0]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABDE = sme.getMatchingSubscriptionInfos("A.B.D.E");
        assertEqual(matchesABDE.size(), (size_t)1);
        assertEqual(matchesABDE[0]->getSubscriptionID(), (uint32_t)456);

        // We check for matches...
        auto matchesABDEF = sme.getMatchingSubscriptionInfos("A.B.D.E.F");
        assertEqual(matchesABDEF.size(), (size_t)1);
        assertEqual(matchesABDEF[0]->getSubscriptionID(), (uint32_t)567);

        // We check for matches...
        auto matchesAB = sme.getMatchingSubscriptionInfos("A.B");
        assertEqual(matchesAB.size(), (size_t)0);

        // We check for matches...
        auto matchesABCDE = sme.getMatchingSubscriptionInfos("A.B.C.D.E");
        assertEqual(matchesABCDE.size(), (size_t)0);
    }

    TestUtils::log("Remove subscriptions...");
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);
        sme.addSubscription("A.B.C", 345, "ClientC", nullptr);
        sme.addSubscription("A.B.C.D", 456, "ClientA", nullptr);
        sme.addSubscription("A.B.C.D", 567, "ClientB", nullptr);

        // We remove subscriptions...
        sme.removeSubscription("A.B.C", "ClientB");
        sme.removeSubscription("A.B.C.D", "ClientA");

        // We check for matches...
        auto matchesABC = sme.getMatchingSubscriptionInfos("A.B.C");
        assertEqual(matchesABC.size(), (size_t)2);
        assertEqual(matchesABC[0]->getSubscriptionID(), (uint32_t)123);
        assertEqual(matchesABC[1]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABCD = sme.getMatchingSubscriptionInfos("A.B.C.D");
        assertEqual(matchesABCD.size(), (size_t)1);
        assertEqual(matchesABCD[0]->getSubscriptionID(), (uint32_t)567);
    }

    TestUtils::log("Remove all subscriptions...");
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);
        sme.addSubscription("A.B.C", 345, "ClientC", nullptr);
        sme.addSubscription("A.B.C.D", 456, "ClientA", nullptr);
        sme.addSubscription("A.B.C.D", 567, "ClientB", nullptr);

        // We remove subscriptions...
        sme.removeAllSubscriptions("ClientA");

        // We check for matches...
        auto matchesABC = sme.getMatchingSubscriptionInfos("A.B.C");
        assertEqual(matchesABC.size(), (size_t)2);
        assertEqual(matchesABC[0]->getSubscriptionID(), (uint32_t)234);
        assertEqual(matchesABC[1]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABCD = sme.getMatchingSubscriptionInfos("A.B.C.D");
        assertEqual(matchesABCD.size(), (size_t)1);
        assertEqual(matchesABCD[0]->getSubscriptionID(), (uint32_t)567);
    }

    TestUtils::log("Wildcard '>'...");
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);
        sme.addSubscription("A.B.>", 345, "ClientC", nullptr);
        sme.addSubscription("A.>", 456, "ClientD", nullptr);
        sme.addSubscription(">", 567, "ClientC", nullptr);
        sme.addSubscription("A.B.D.>", 678, "ClientF", nullptr);

        // We check for matches...
        auto matchesABC = sme.getMatchingSubscriptionInfos("A.B.C");
        assertEqual(matchesABC.size(), (size_t)5);
        assertEqual(containsID(matchesABC, 123), 123);
        assertEqual(containsID(matchesABC, 234), 234);
        assertEqual(containsID(matchesABC, 345), 345);
        assertEqual(containsID(matchesABC, 456), 456);
        assertEqual(containsID(matchesABC, 567), 567);

        auto matchesABD = sme.getMatchingSubscriptionInfos("A.B.D");
        assertEqual(matchesABD.size(), (size_t)3);
        assertEqual(containsID(matchesABD, 345), 345);
        assertEqual(containsID(matchesABD, 456), 456);
        assertEqual(containsID(matchesABD, 567), 567);

        auto matchesXYZ = sme.getMatchingSubscriptionInfos("X.Y.Z");
        assertEqual(matchesXYZ.size(), (size_t)1);
        assertEqual(containsID(matchesXYZ, 567), 567);

        auto matchesABDXYZ = sme.getMatchingSubscriptionInfos("A.B.D.X.Y.Z");
        assertEqual(matchesABDXYZ.size(), (size_t)4);
        assertEqual(containsID(matchesABDXYZ, 345), 345);
        assertEqual(containsID(matchesABDXYZ,456), 456);
        assertEqual(containsID(matchesABDXYZ, 567), 567);
        assertEqual(containsID(matchesABDXYZ, 678), 678);
    }
}

// Returns the subscription ID (as an int) is the collection conatins it, -1 if not.
int Tests_Gateway::containsID(const VecSubscriptionInfo& subscriptionInfos, uint32_t subscriptionID)
{
    for (const auto& pSubscriptionInfo : subscriptionInfos)
    {
        if (pSubscriptionInfo->getSubscriptionID() == subscriptionID)
        {
            return subscriptionID;
        }
    }
    return -1;
}
