#include "Tests_Gateway.h"
#include "TestUtils.h"
#include "SubjectMatchingEngine.h"
using namespace MessagingMesh;

// Test for the subject-matching engine.
void Tests_Gateway::subjectMatchingEngine()
{
    // Single subscription...
    {
        SubjectMatchingEngine sme;

        // We add subscriptions...
        sme.addSubscription("A.B.C", 123, "ClientA", nullptr);
        sme.addSubscription("A.B.C", 234, "ClientB", nullptr);

        // We check for matches...

    }

}
