#include "SubjectMatchingEngine.h"
#include "MMUtils.h"
using namespace MessagingMesh;

// Adds a subscription.
void SubjectMatchingEngine::addSubscription(const std::string& subject, uint32_t /*subscriptionID*/, Socket* /*pClientSocket*/)
{
    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');
}
