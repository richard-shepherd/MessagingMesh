#include "Tests_Gateway.h"
#include <Tests_MessagingMeshLib.h>
#include <TestUtils.h>
#include "SubjectMatchingEngine.h"
#include "SubscriptionInfo.h"
using namespace MessagingMesh;
using namespace MessagingMesh::TestUtils;

// Runs all tests.
void Tests_Gateway::runAll()
{
    TestUtils::TestRun testRun;

    Tests_MessagingMeshLib::runAll(testRun);
    Tests_Gateway::subjectMatchingEngine(testRun);
}

// Tests for the subject-matching engine.
void Tests_Gateway::subjectMatchingEngine(TestRun& testRun)
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
        assertEqual(testRun, matchesABC.size(), (size_t)2);
        assertEqual(testRun, matchesABC[0]->getSubscriptionID(), (uint32_t)123);
        assertEqual(testRun, matchesABC[1]->getSubscriptionID(), (uint32_t)234);

        // We check for matches...
        auto matchesABD = sme.getMatchingSubscriptionInfos("A.B.D");
        assertEqual(testRun, matchesABD.size(), (size_t)1);
        assertEqual(testRun, matchesABD[0]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABDE = sme.getMatchingSubscriptionInfos("A.B.D.E");
        assertEqual(testRun, matchesABDE.size(), (size_t)1);
        assertEqual(testRun, matchesABDE[0]->getSubscriptionID(), (uint32_t)456);

        // We check for matches...
        auto matchesABDEF = sme.getMatchingSubscriptionInfos("A.B.D.E.F");
        assertEqual(testRun, matchesABDEF.size(), (size_t)1);
        assertEqual(testRun, matchesABDEF[0]->getSubscriptionID(), (uint32_t)567);

        // We check for matches...
        auto matchesAB = sme.getMatchingSubscriptionInfos("A.B");
        assertEqual(testRun, matchesAB.size(), (size_t)0);

        // We check for matches...
        auto matchesABCDE = sme.getMatchingSubscriptionInfos("A.B.C.D.E");
        assertEqual(testRun, matchesABCDE.size(), (size_t)0);
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
        assertEqual(testRun, matchesABC.size(), (size_t)2);
        assertEqual(testRun, matchesABC[0]->getSubscriptionID(), (uint32_t)123);
        assertEqual(testRun, matchesABC[1]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABCD = sme.getMatchingSubscriptionInfos("A.B.C.D");
        assertEqual(testRun, matchesABCD.size(), (size_t)1);
        assertEqual(testRun, matchesABCD[0]->getSubscriptionID(), (uint32_t)567);
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
        assertEqual(testRun, matchesABC.size(), (size_t)2);
        assertEqual(testRun, matchesABC[0]->getSubscriptionID(), (uint32_t)234);
        assertEqual(testRun, matchesABC[1]->getSubscriptionID(), (uint32_t)345);

        // We check for matches...
        auto matchesABCD = sme.getMatchingSubscriptionInfos("A.B.C.D");
        assertEqual(testRun, matchesABCD.size(), (size_t)1);
        assertEqual(testRun, matchesABCD[0]->getSubscriptionID(), (uint32_t)567);
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
        assertEqual(testRun, matchesABC.size(), (size_t)5);
        assertEqual(testRun, containsID(matchesABC, 123), 123);
        assertEqual(testRun, containsID(matchesABC, 234), 234);
        assertEqual(testRun, containsID(matchesABC, 345), 345);
        assertEqual(testRun, containsID(matchesABC, 456), 456);
        assertEqual(testRun, containsID(matchesABC, 567), 567);

        auto matchesABD = sme.getMatchingSubscriptionInfos("A.B.D");
        assertEqual(testRun, matchesABD.size(), (size_t)3);
        assertEqual(testRun, containsID(matchesABD, 345), 345);
        assertEqual(testRun, containsID(matchesABD, 456), 456);
        assertEqual(testRun, containsID(matchesABD, 567), 567);

        auto matchesXYZ = sme.getMatchingSubscriptionInfos("X.Y.Z");
        assertEqual(testRun, matchesXYZ.size(), (size_t)1);
        assertEqual(testRun, containsID(matchesXYZ, 567), 567);

        auto matchesABDXYZ = sme.getMatchingSubscriptionInfos("A.B.D.X.Y.Z");
        assertEqual(testRun, matchesABDXYZ.size(), (size_t)4);
        assertEqual(testRun, containsID(matchesABDXYZ, 345), 345);
        assertEqual(testRun, containsID(matchesABDXYZ,456), 456);
        assertEqual(testRun, containsID(matchesABDXYZ, 567), 567);
        assertEqual(testRun, containsID(matchesABDXYZ, 678), 678);
    }

    TestUtils::log("Wildcard '*'...");
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);
        sme.addSubscription("A.B.*", 345, "ClientC", nullptr);
        sme.addSubscription("A.*.C", 456, "ClientD", nullptr);
        sme.addSubscription("*", 567, "ClientC", nullptr);
        sme.addSubscription("*.>", 678, "ClientF", nullptr);

        // We check for matches...
        auto matchesABC = sme.getMatchingSubscriptionInfos("A.B.C");
        assertEqual(testRun, matchesABC.size(), (size_t)5);
        assertEqual(testRun, containsID(matchesABC, 123), 123);
        assertEqual(testRun, containsID(matchesABC, 234), 234);
        assertEqual(testRun, containsID(matchesABC, 345), 345);
        assertEqual(testRun, containsID(matchesABC, 456), 456);
        assertEqual(testRun, containsID(matchesABC, 678), 678);

        // We check for matches...
        auto matchesA = sme.getMatchingSubscriptionInfos("A");
        assertEqual(testRun, matchesA.size(), (size_t)1);
        assertEqual(testRun, containsID(matchesA, 567), 567);

        // We check for matches...
        auto matchesAXQ = sme.getMatchingSubscriptionInfos("A.X.Q");
        assertEqual(testRun, matchesAXQ.size(), (size_t)1);
        assertEqual(testRun, containsID(matchesAXQ, 678), 678);
    }
}

// Returns the subscription ID (as an int) if the collection contains it, -1 if not.
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
