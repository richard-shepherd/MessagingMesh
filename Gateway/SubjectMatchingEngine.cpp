#include "SubjectMatchingEngine.h"
#include <MMUtils.h>
#include <Socket.h>
#include <Logger.h>
#include "SubscriptionInfo.h"
using namespace MessagingMesh;

// Adds a subscription.
// Returns the number of clients registered for this subject.
size_t SubjectMatchingEngine::addSubscription(const std::string& subject, uint32_t subscriptionID, uint64_t clientSocketID, Socket* pClientSocket)
{
    // We get the node for the subject...
    auto pNode = getOrCreateNode(subject);

    // We add subscription-info.
    // Note: We are not expecting more than one subscription from a client
    //       for the same subject. This is managed in client libraries.
    auto pSubscriptionInfo = SubscriptionInfo::create(pClientSocket, subscriptionID);
    pNode->SubscriptionInfos.insert({ clientSocketID, pSubscriptionInfo });

    // The change to subscriptions has invalidated the cache...
    m_cache.clear();

    // We returns the number of clients registered for the subject...
    return pNode->SubscriptionInfos.size();
}

// Removes a subscription.
// Returns the number of clients registered for this subject.
size_t SubjectMatchingEngine::removeSubscription(const std::string& subject, uint64_t clientSocketID)
{
    // We get the node for the subject...
    auto pNode = getOrCreateNode(subject);

    // We remove info for this client...
    pNode->SubscriptionInfos.erase(clientSocketID);

    // RSSTODO: Clean up nodes which are no longer used by any subscriptions.

    // The change to subscriptions has invalidated the cache...
    m_cache.clear();

    // We returns the number of clients registered for the subject...
    return pNode->SubscriptionInfos.size();
}

// Removes all subscriptions for the client specified.
void SubjectMatchingEngine::removeAllSubscriptions(uint64_t clientSocketID)
{
    removeAllSubscriptions(m_pRootNode, clientSocketID);

    // The change to subscriptions has invalidated the cache...
    m_cache.clear();
}

// Removes all subscriptions for the client specified from the node provided
// and from all its child nodes recursively.
void SubjectMatchingEngine::removeAllSubscriptions(Node* pNode, uint64_t clientSocketID)
{
    // We remove subscriptions from this node...
    pNode->SubscriptionInfos.erase(clientSocketID);

    // We remove subscriptions (recursively) from child nodes for non-wildcard tokens...
    for (const auto& pair : pNode->Nodes)
    {
        removeAllSubscriptions(pair.second, clientSocketID);
    }

    // We remove subscriptions (recursively) from wildcard tokens...
    if (pNode->pNode_Wildcard_Star)
    {
        removeAllSubscriptions(pNode->pNode_Wildcard_Star, clientSocketID);
    }
    if (pNode->pNode_Wildcard_GreaterThan)
    {
        removeAllSubscriptions(pNode->pNode_Wildcard_GreaterThan, clientSocketID);
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
    VecSubscriptionInfo results;

    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');
    auto tokenCount = tokens.size();
    if (tokenCount != 0)
    {
        // We match tokens against the interest graph...
        auto lastTokenIndex = tokenCount - 1;
        getMatchingSubscriptionInfos(m_pRootNode, tokens, 0, lastTokenIndex, results);
    }

    // We add the results to the cache...
    if (m_cachingEnabled)
    {
        m_cache.insert({ subject, results });
    }

    return results;
}

// Checks the current node for matching subscriptions.
void SubjectMatchingEngine::getMatchingSubscriptionInfos(Node* pNode, const VecToken& tokens, size_t tokenIndex, size_t lastTokenIndex, VecSubscriptionInfo& subscriptionInfos)
{
    // We find the current token and check if this node contains it...
    const auto& token = tokens[tokenIndex];
    auto it = pNode->Nodes.find(token);
    if (it != pNode->Nodes.end())
    {
        // The token is in the node. 
        auto pChildNode = it->second;
        if (tokenIndex == lastTokenIndex)
        {
            // This is the last token, so we add the subscription-infos to the results...
            addSubscriptionInfos(pChildNode, subscriptionInfos);
        }
        else
        {
            // This is not the last token, so we continue walking the graph...
            getMatchingSubscriptionInfos(pChildNode, tokens, tokenIndex + 1, lastTokenIndex, subscriptionInfos);
        }
    }

    // We check if the node has the '>' wildcard...
    if (pNode->pNode_Wildcard_GreaterThan)
    {
        // We have a '>' subscription. In this case we add the subscription infos
        // from the node without needing the walk the graph further...
        addSubscriptionInfos(pNode->pNode_Wildcard_GreaterThan, subscriptionInfos);
    }

    // We check if the node has the '*' wildcard...
    if (pNode->pNode_Wildcard_Star)
    {
        // There is a '*' wildcard
        auto pChildNode = pNode->pNode_Wildcard_Star;
        if (tokenIndex == lastTokenIndex)
        {
            // This is the last token, so we add the subscription-infos to the results...
            addSubscriptionInfos(pChildNode, subscriptionInfos);
        }
        else
        {
            // This is not the last token, so we continue walking the graph...
            getMatchingSubscriptionInfos(pChildNode, tokens, tokenIndex + 1, lastTokenIndex, subscriptionInfos);
        }
    }
}

// Adds all subscription infos from the node to the vector.
void SubjectMatchingEngine::addSubscriptionInfos(Node* pNode, VecSubscriptionInfo& subscriptionInfos)
{
    for (const auto& pair : pNode->SubscriptionInfos)
    {
        subscriptionInfos.push_back(pair.second);
    }
}

// Gets the node in the interest graph for the subject specified.
// Creates nodes in the graph if necessary.
SubjectMatchingEngine::Node* SubjectMatchingEngine::getOrCreateNode(const std::string& subject)
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
                nodeMap.insert({ std::string(token), pNode });
            }
            else
            {
                pNode = it->second;
            }
        }
    }

    return pNode;
}
