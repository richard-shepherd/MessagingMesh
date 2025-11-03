#pragma once
#include <cstdint>
#include <string>

namespace MessagingMesh
{
    // Forward declarations...
    class Socket;

    /// <summary>
    /// Matches subjects in sent messages to clients which are subscribed to them.
    /// 
    /// Interest graph
    /// --------------
    /// Matching is done using an interest graph. This is a map of maps, where
    /// each level of the grap processes one token of the subject.
    /// 
    /// Subscriptions
    /// -------------
    /// When a client subscribes to a subject we set this up in the graph:
    /// - The subject is split into tokens by the '.' delimiter.
    /// - The tokens are added at each level of the graph.
    /// - Client-info, including its socket, is added at the point in the
    ///   graph for the last token.
    /// 
    /// Example 1:
    /// - Client-A subscribes to A.B.C
    /// - A -> B -> C = Client-A
    /// 
    /// - Client-B subscribes to A.B.D.E
    /// - A -> B -> C = Client-A
    ///          -> D -> E = Client-B
    /// 
    /// Matching sent subjects
    /// ----------------------
    /// RULE:
    /// - Split the subject into tokens by the '.' delimiter.
    /// - Match each token to nodes in the graph.
    /// - Matching client-infos are taken from the node corresponding to the last token.
    /// 
    /// With Example 1 above:
    /// - A.B.C => CLient-A
    /// - A.B.C.D => no-match
    /// 
    /// Wildcard *
    /// ----------
    /// Subscriptions can include the * wildcard. This matches any single token at the
    /// point at which the * occurs. For example:
    /// - A.*.C matches A.B.C and A.D.C, but does not match A.B or A.B.C.D
    /// 
    /// RULE: To handle this we check for both the token itself and * at each level of the graph.
    ///       We follow each match down the remainder of the graph, for each token in the subject
    ///       being matched.
    /// 
    /// Example 2:
    /// - We have the graph from Example 1 with these additions.
    /// 
    /// - Client-C subscribes to A.X.C
    /// - A -> B -> C = Client-A
    ///          -> D -> E = Client-B
    ///     -> X -> C = Client-C
    /// 
    /// - Client-D subscribes to A.*.C
    /// - A -> B -> C = Client-A
    ///          -> D -> E = Client-B
    ///     -> * -> C = Client-D
    ///     -> X -> C = Client-C
    /// 
    /// - A client publishes A.B.C.
    /// 
    /// - We start at the root of the graph.
    /// - Check for A and * -> A
    ///   - A: Check for B and * -> B and *
    ///     - B: Check for C and * -> C = Client-A
    ///     - *: Check for C and * -> C = Client-D
    /// 
    /// Wildcard >
    /// ----------
    /// The wildcard > always comes at the end of a subscribe subject and matches all 
    /// tokens which come at its point or after it. For example:
    /// - A.B.> matches A.B.C and A.B.X.Y, but does not match A.B.
    ///   
    /// RULE: At each level of the graph we check for > as well as the current token and *.
    ///       If we find > we have an immediate match without checking further tokens.
    ///       We still check all remaining tokens as there may be matches to other subscriptions.
    /// 
    /// Cached lookups
    /// --------------
    /// Caching can be enabled or disabled with the enableCaching(true/false) method.
    /// If caching is enabled we store the results of matches (found via the interest 
    /// graph) in a map of subject to client-infos. 
    /// 
    /// When matching a subject, we first check the cache and only use the interest graph
    /// if no match is found.
    /// 
    /// We clear or modify the cache as subscriptions are added or removed.
    /// </summary>
    class SubjectMatchingEngine
    {
    // Public types
    public:
        struct ClientInfo
        {
            Socket* pSocket;
            uint32_t subscriptionID;
        };

    // Public methods...
    public:
        // Adds a subscription.
        void addSubscription(const std::string& subject, uint32_t subscriptionID, Socket* pClientSocket);

    // Private data...
    private:
        // A node in the interest graph.
        struct Node
        {

        };
    };
} // namespace


