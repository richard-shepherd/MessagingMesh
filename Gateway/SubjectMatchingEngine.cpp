#include "SubjectMatchingEngine.h"
#include "MMUtils.h"
#include "Socket.h"
#include "SubscriptionInfo.h"
#include "Logger.h"
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

    // The change to subscriptions has invalidaed the cache...
    m_cache.clear();
}

// Removes a subscription.
void SubjectMatchingEngine::removeSubscription(const std::string& subject, const std::string& clientName)
{
    // We get the node for the subject...
    auto pNode = getNode(subject);

    // We remove info for this client...
    pNode->SubscriptionInfos.erase(clientName);

    // The change to subscriptions has invalidaed the cache...
    m_cache.clear();
}

// Removes all subscriptions for the client specified.
void SubjectMatchingEngine::removeAllSubscriptions(const std::string& clientName)
{
    removeAllSubscriptions(m_pRootNode, clientName);

    // The change to subscriptions has invalidaed the cache...
    m_cache.clear();
}

// Removes all subscriptions for the client specified from the node provided
// and from all its child nodes recursively.
void SubjectMatchingEngine::removeAllSubscriptions(Node* pNode, const std::string& clientName)
{
    pNode->SubscriptionInfos.erase(clientName);
    for (const auto& pair : pNode->Nodes)
    {
        removeAllSubscriptions(pair.second, clientName);
    }
}

// Returns subscription-infos that match the subject provided.
VecSubscriptionInfo SubjectMatchingEngine::getMatchingSubscriptionInfos(const std::string& subject)
{
    // We check if we have the matches cached...
    if (m_cachingEnabled)
    {
        auto it = m_cache.find(subject);
        if (it != m_cache.end())
        {
            return it->second;
        }
    }

    // Caching is not enabled, or we did not find a match in the cache. So we
    // look for matching subscriptions...
    std::vector<SubscriptionInfoPtr> results;

    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');

    // We match each token in the interest graph. 
    // When all tokens have matched, we return the subscription-infos from the final node.
    auto gotMatch = true;
    auto pNode = m_pRootNode;
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
        pNode = it->second;
    }

    // If we have a match, we return the collection of subscription-infos...
    if (gotMatch)
    {
        for (const auto& pair : pNode->SubscriptionInfos)
        {
            results.push_back(pair.second);
        }
    }

    // We add the results to the cache...
    if (m_cachingEnabled)
    {
        m_cache.insert({ subject, results });
    }

    return results;
}

// Gets the node in the interest graph for the subject specified.
SubjectMatchingEngine::Node* SubjectMatchingEngine::getNode(const std::string& subject)
{
    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');

    // We find the node for the subject by walking the graph for each token...
    auto pNode = m_pRootNode;
    for (const auto& token : tokens)
    {
        if (token == WILDCARD_STAR)
        {
            // The token is is "*" wildcard...
            if (!pNode->pNode_Wildcard_Star)
            {
                pNode->pNode_Wildcard_Star = new Node;
            }
            pNode = pNode->pNode_Wildcard_Star;
        }
        else if (token == WILDCARD_GREATER_THAN)
        {
            // The token is is ">" wildcard...
            if (!pNode->pNode_Wildcard_GreaterThan)
            {
                pNode->pNode_Wildcard_GreaterThan = new Node;
            }
            pNode = pNode->pNode_Wildcard_GreaterThan;
        }
        else
        {
            // We have a non-wildcard token.
            // We find or create the node for the token.
            auto& nodeMap = pNode->Nodes;
            auto it = nodeMap.find(token);
            if (it == nodeMap.end())
            {
                // There is no node for the token, so we create it...
                pNode = new Node;
                nodeMap.insert({ token, pNode });
            }
            else
            {
                pNode = it->second;
            }
        }
    }

    return pNode;
}
