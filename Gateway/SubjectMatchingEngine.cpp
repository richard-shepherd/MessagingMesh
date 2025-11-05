#include "SubjectMatchingEngine.h"
#include "MMUtils.h"
#include "Socket.h"
#include "SubscriptionInfo.h"
using namespace MessagingMesh;

// Adds a subscription.
void SubjectMatchingEngine::addSubscription(const std::string& subject, uint32_t subscriptionID, const std::string& clientName, Socket* pClientSocket)
{
    // We get the node for the subject...
    auto pNode = getNode(subject);

    // We add subscription-info.
    // Note: We are not expecting more than one subscription from a client
    //       for the same subject. This is managed in client libraries.
    auto pSubscriptionInfo = SubscriptionInfo::create(pClientSocket, subscriptionID);
    pNode->SubscriptionInfos.insert({clientName, pSubscriptionInfo});
}

// Removes a subscription.
void SubjectMatchingEngine::removeSubscription(const std::string& subject, const std::string& clientName)
{
    // We get the node for the subject...
    auto pNode = getNode(subject);

    // We remove info for this client...
    pNode->SubscriptionInfos.erase(clientName);
}

// Returns subscription-infos that match the subject provided.
std::vector<SubscriptionInfoPtr> SubjectMatchingEngine::getMatchingSubscriptionInfos(const std::string& subject)
{
    std::vector<SubscriptionInfoPtr> results;

    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');

    // We match each token in the interest graph. 
    // When all tokens have matched, we return the subscription-infos from the final node.
    auto gotMatch = true;
    auto pNode = &m_rootNode;
    for (auto& token : tokens)
    {
        // We check for a matching token...
        auto it = pNode->Nodes.find(token);
        if (it == pNode->Nodes.end())
        {
            // We did not find a match...
            gotMatch = false;
            break;
        }

        // We move to the next node...
        pNode = &(it->second);
    }

    // If we have a match, we return the collection of subscription-infos...
    if (gotMatch)
    {
        for (const auto& pair : pNode->SubscriptionInfos)
        {
            results.push_back(pair.second);
        }
    }

    return results;
}

// Gets the node in the interest graph for the subject specified.
SubjectMatchingEngine::Node* SubjectMatchingEngine::getNode(const std::string& subject)
{
    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');

    // We find the node for the subject by walking the graph for each token...
    auto pNode = &m_rootNode;
    for (auto& token : tokens)
    {
        // Finds or creates the node for the token...
        pNode = &(pNode->Nodes[token]);
    }

    return pNode;
}
