#include <memory>
#include <vector>

namespace MessagingMesh
{
    // Forward declarations...
    class SubscriptionInfo;

    // Shared pointer to a SubscriptionInfo.
    typedef std::shared_ptr<SubscriptionInfo> SubscriptionInfoPtr;

    // Vector of SubscriptionInfo.
    typedef std::vector<SubscriptionInfoPtr> VecSubscriptionInfo;

} // namespace
