#include <memory>
#include <vector>

namespace MessagingMesh
{
    // Forward declarations...
    class SubscriptionInfo;

    // Shared pointer to a SubscriptionInfo.
    using SubscriptionInfoPtr = std::shared_ptr<SubscriptionInfo>;

    // Vector of SubscriptionInfo.
    using VecSubscriptionInfo = std::vector<SubscriptionInfoPtr>;

} // namespace
