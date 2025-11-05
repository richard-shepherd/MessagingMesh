#include "Tests_Gateway.h"
#include "TestUtils.h"
#include "SubjectMatchingEngine.h"
#include "SubscriptionInfo.h"
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

    TestUtils::log("Removing subscriptions...");
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
}
