#include "SubjectMatchingEngine.h"
#include "MMUtils.h"
#include "Socket.h"
using namespace MessagingMesh;

// Adds a subscription.
void SubjectMatchingEngine::addSubscription(const std::string& subject, uint32_t subscriptionID, const std::string& clientName, Socket* pClientSocket)
{
    // We tokenize the subject...
    auto tokens = MMUtils::tokenize(subject, '.');

    // We update the interest graph...
    auto pNode = &m_rootNode;
    for (auto& token : tokens)
    {
        // Finds or creates the node for the token...
        pNode = &(pNode->Nodes[token]);
    }

    // We add client-info for the subscription to the last node in the chain.
    // Note: We are not expecting more than one subscription from a client
    //       for the same subject. This is managed in client libraries.
    ClientInfo clientInfo;
    clientInfo.pSocket = pClientSocket;
    clientInfo.subscriptionID = subscriptionID;
    pNode->ClientInfos.insert({clientName, clientInfo});
}
